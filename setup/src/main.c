/*
 * Copyright (c) 2003-2006, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
 #define INITGUID
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include <tchar.h>
#include <shlguid.h>
#include <olectl.h>
#include <objidl.h>
#include <shlwapi.h>
#include <objbase.h>
#include <initguid.h>

#include <layout.h>
#include "page.h"

#include "resource.h"

typedef struct {
  int x;
  int y;
  int width;
  int height;
} WINDOW_PLACEMENT;

static BOOL CALLBACK main_dialog_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param);
extern PAGE* create_options_page(HANDLE resource, HWND parent);
extern PAGE* create_debug_page(HANDLE resource, HWND parent);
extern void about(HANDLE resource, HWND parent);

static HANDLE resource;
static WINDOW_PLACEMENT* window_placement = 0;

static WNDPROC old_button_procedure;

static LRESULT
new_button_procedure(HWND button, UINT message, WPARAM w_param, LPARAM l_param) {
  LRESULT result= CallWindowProc(old_button_procedure, button, message, w_param, l_param);
  if(message == WM_LBUTTONUP) {
    MessageBox(0, TEXT("A button has been clicked"), TEXT("Info"), MB_OK);
  }
  
  return result;
}

static void
subclass_button() {
  HWND tmp;
  
  tmp = CreateWindow(TEXT("BUTTON"), TEXT("tmp"), WS_POPUP, 0, 0, 50, 50, 0, NULL, GetModuleHandle(0), NULL);
  
  old_button_procedure = (WNDPROC)SetClassLongPtr(tmp, GCLP_WNDPROC, (LONG_PTR)new_button_procedure);
  
  DestroyWindow(tmp);
}

DEFINE_GUID(CLSID_DIFF_EXT, 0xA0482097, 0xC69D, 0x4DEC, 0x8A, 0xB6, 0xD3, 0xA2, 0x59, 0xAC, 0xC1, 0x51);

/* Move to utils */
#define PACKVERSION(major,minor) MAKELONG(minor,major)
DWORD GetDllVersion(LPCTSTR lpszDllName)
{
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;

    /* For security purposes, LoadLibrary should be provided with a 
       fully-qualified path to the DLL. The lpszDllName variable should be
       tested to ensure that it is a fully qualified path before it is used. */
    hinstDll = LoadLibrary(lpszDllName);
	
    if(hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");

        /* Because some DLLs might not implement this function, you
        must test for it explicitly. Depending on the particular 
        DLL, the lack of a DllGetVersion function can be a useful
        indicator of the version. */

        if(pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;

            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);

            hr = (*pDllGetVersion)(&dvi);

            if(SUCCEEDED(hr))
            {
               dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }
        }

        FreeLibrary(hinstDll);
    }
    
    return dwVersion;
}

#ifdef __MINGW32__
/*
Wait for wide startup module from MinGW
*/
int APIENTRY
WinMain(HINSTANCE instance, HINSTANCE not_used1, LPSTR not_used2, int not_used3) {
#else  
int APIENTRY
_tWinMain(HINSTANCE instance, HINSTANCE not_used1, LPSTR not_used2, int not_used3) {
#endif  
  HRESULT exit = ID_APPLY;
  HKEY key;
  DWORD language = 0;
  DWORD hlen;
  TCHAR language_lib[MAX_PATH];
  HGLOBAL dialog_handle;
  HRSRC resource_handle;
  DLGTEMPLATE* dialog;
  
/*  subclass_button();*/

  CoInitialize(0);
  SetThreadLocale(LOCALE_USER_DEFAULT);
  InitCommonControls();
  
  while(exit == ID_APPLY) {
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
      hlen = sizeof(DWORD);
      RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&language, &hlen);
  
      RegCloseKey(key);
  
      _stprintf(language_lib, TEXT("diff_ext_setup%ld.dll"), language);
    }
  
    resource = LoadLibrary(language_lib);
  
    if(resource == NULL) {
      resource = instance;
    }
  
    resource_handle = FindResource(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), RT_DIALOG);

    dialog_handle = LoadResource(resource, resource_handle);
    dialog = (DLGTEMPLATE*)LockResource(dialog_handle);
  
  /*  can not do because have to specify hinst as module instance and load dialog from translated resource only dll...
    ret = DialogBox(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)main_dialog_func);
  */  
    exit = (int)DialogBoxIndirect(instance, dialog, NULL, (DLGPROC)main_dialog_func);

    FreeResource(dialog);
    FreeLibrary(resource);
  }
  
  CoUninitialize();
  
  return exit;
}

static void
init(HWND dialog, WPARAM not_used1, LPARAM not_used2) {
  HKEY key;
  TCHAR command[4*MAX_PATH] = TEXT("");
  TCHAR command3[6*MAX_PATH] = TEXT("");
  TCHAR home[MAX_PATH] = TEXT(".");
  DWORD language = 1033;
  DWORD three_way_compare_supported = 0;
  HANDLE file;
  WIN32_FIND_DATA file_info;
  TCHAR prefix[] = TEXT("diff_ext");
  TCHAR root[] = TEXT("????");
  TCHAR suffix[] = TEXT(".dll");
  TCHAR* locale_info;
  int locale_info_size;
  LRESULT curr = 0;  
  LPWSTR  tmp_guid;
  TCHAR class_id[MAX_PATH];
  
  if(StringFromIID(&CLSID_DIFF_EXT, &tmp_guid) == S_OK) {
    TCHAR clsid[MAX_PATH] = TEXT("");
#ifdef UNICODE    
    _tcsncpy(class_id, tmp_guid, MAX_PATH);
#else
    wcstombs(class_id, tmp_guid, MAX_PATH);
#endif
    CoTaskMemFree((void*)tmp_guid);
    
    if(RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("Folder\\shellex\\ContextMenuHandlers\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
      DWORD hlen = MAX_PATH;
    
      RegQueryValueEx(key, TEXT(""), 0, NULL, (BYTE*)clsid, &hlen);
      
      if(_tcsncmp(clsid, class_id, MAX_PATH) == 0) {
        SendDlgItemMessage(dialog, ID_DIFF_DIRS, BM_SETCHECK, BST_CHECKED, 0);
      }
      
      RegCloseKey(key);
    }
    
    if(RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("Directory\\shellex\\ContextMenuHandlers\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
      DWORD hlen = MAX_PATH;
    
      RegQueryValueEx(key, TEXT(""), 0, NULL, (BYTE*)clsid, &hlen);
      
      if(_tcsncmp(clsid, class_id, MAX_PATH) == 0) {
        SendDlgItemMessage(dialog, ID_DIFF_DIRS, BM_SETCHECK, BST_CHECKED, 0);
      }
      
      RegCloseKey(key);
    }
  }
  
  if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
    DWORD hlen = 4*MAX_PATH;
  
    RegQueryValueEx(key, TEXT("diff"), 0, NULL, (BYTE*)command, &hlen);
    hlen = 6*MAX_PATH;
    RegQueryValueEx(key, TEXT("diff3"), 0, NULL, (BYTE*)command3, &hlen);

    hlen = sizeof(DWORD);
    if(RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)(&language), &hlen) != ERROR_SUCCESS) {
      language = 1033;
    }
    if(RegQueryValueEx(key, TEXT("3way_compare_supported"), 0, NULL, (BYTE*)(&three_way_compare_supported), &hlen) != ERROR_SUCCESS) {
      three_way_compare_supported = 0;
    }

    hlen = MAX_PATH;
    if(RegQueryValueEx(key, TEXT("home"), 0, NULL, (BYTE*)home, &hlen) != ERROR_SUCCESS) {
      lstrcpy(home, TEXT("."));
    }

    RegCloseKey(key);
  }
  
  locale_info_size = GetLocaleInfo(1033, LOCALE_SNATIVELANGNAME, 0, 0);
  locale_info = (TCHAR*)malloc(locale_info_size*sizeof(TCHAR));
  GetLocaleInfo(1033, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

  curr = SendDlgItemMessage(dialog, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
  SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETITEMDATA, curr, 1033);

  if(language == 1033) {
    SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETCURSEL, curr, 0);
  }

  free(locale_info); 

  lstrcat(home, TEXT("\\"));
  lstrcat(home, prefix);
  lstrcat(home, root);
  lstrcat(home, suffix);

  file = FindFirstFile(home, &file_info);
  if(file != INVALID_HANDLE_VALUE) {
    BOOL stop = FALSE;

    while(stop == FALSE) {
      DWORD lang_id = 0;
      int result;
      
      result = _stscanf(file_info.cFileName, TEXT("diff_ext%4u.dll"), &lang_id);

      if(result > 0) {
        locale_info_size = GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, 0, 0);
        locale_info = (TCHAR*)malloc(locale_info_size*sizeof(TCHAR));
        GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

        curr = SendDlgItemMessage(dialog, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
        SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETITEMDATA, curr, lang_id);

        if(lang_id == language) {
          SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETCURSEL, curr, 0);
        }

        free(locale_info);
      }

      stop = !FindNextFile(file, &file_info);
    }

    FindClose(file);
  }
  
  if(three_way_compare_supported != 0) {
    SendDlgItemMessage(dialog, ID_DIFF3, BM_SETCHECK, BST_CHECKED, 0);
  }
  SetDlgItemText(dialog, ID_DIFF_COMMAND, command);
  SetDlgItemText(dialog, ID_COMMAND_DIFF3, command3);
  SendDlgItemMessage(dialog, ID_DIFF_COMMAND, EM_SETLIMITTEXT, 4*MAX_PATH, 0);
  SendDlgItemMessage(dialog, ID_COMMAND_DIFF3, EM_SETLIMITTEXT, 6*MAX_PATH, 0);
  
  if(GetDllVersion(TEXT("shlwapi.dll")) >= PACKVERSION(5,0)) {
    HMODULE libshlwapi = LoadLibrary(TEXT("shlwapi.dll"));
    
    if(libshlwapi != 0) {
      HRESULT (WINAPI* SHAutoComplete_fn)(HWND, DWORD) = 0;
      
      SHAutoComplete_fn = (HRESULT (WINAPI*)(HWND, DWORD))GetProcAddress(libshlwapi, "SHAutoComplete");
      
      if(SHAutoComplete_fn != 0) {
        SHAutoComplete_fn(GetDlgItem(dialog, ID_DIFF_COMMAND), 9 /*SHACF_FILESYSTEM | SHACF_USETAB*/);
        SHAutoComplete_fn(GetDlgItem(dialog, ID_COMMAND_DIFF3), 9 /*SHACF_FILESYSTEM | SHACF_USETAB*/);
      }
    }
  }
/******************************************************************************/  
  SetClassLongPtr(dialog, GCLP_HICON, (LONG_PTR)LoadIcon(resource, MAKEINTRESOURCE(MAIN_ICON)));
  
  attach_layout(resource, dialog, MAKEINTRESOURCE(ID_MAINDIALOG_LAYOUT));
  
  if(window_placement == 0) {
    RECT rect;
    window_placement = (WINDOW_PLACEMENT*)malloc(sizeof(WINDOW_PLACEMENT));
    
    GetWindowRect(dialog, &rect);
    window_placement->x = rect.left;
    window_placement->y = rect.top;
    window_placement->width = rect.right-rect.left;
    window_placement->height = rect.bottom-rect.top;
  } else {
    MoveWindow(dialog, window_placement->x, window_placement->y, window_placement->width, window_placement->height, TRUE);
  }
}

static void
apply(HWND dialog) {
  HKEY key;
  TCHAR command[4*MAX_PATH];
  TCHAR command3[6*MAX_PATH];
  DWORD language;
  LRESULT idx;
  LRESULT compare_folders;
  LRESULT three_way_compare_supported;
  DWORD three_way_compare_supported_value = 0;
  
  GetDlgItemText(dialog, ID_DIFF_COMMAND, command, sizeof(command)/sizeof(command[0]));
  GetDlgItemText(dialog, ID_COMMAND_DIFF3, command3, sizeof(command3)/sizeof(command3[0]));
  idx = SendDlgItemMessage(dialog, ID_LANGUAGE, CB_GETCURSEL, 0, 0);
  language = (DWORD)SendDlgItemMessage(dialog, ID_LANGUAGE, CB_GETITEMDATA, idx, 0);
  compare_folders = SendDlgItemMessage(dialog, ID_DIFF_DIRS, BM_GETCHECK, 0, 0);  
  three_way_compare_supported = SendDlgItemMessage(dialog, ID_DIFF3, BM_GETCHECK, 0, 0);  
  if(three_way_compare_supported == BST_CHECKED) {
    three_way_compare_supported_value = 1;
  }

  RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Z\\diff-ext"), 0, KEY_SET_VALUE, &key);
  RegSetValueEx(key, TEXT("diff"), 0, REG_SZ, (const BYTE*)command, lstrlen(command)*sizeof(TCHAR));
  RegSetValueEx(key, TEXT("diff3"), 0, REG_SZ, (const BYTE*)command3, lstrlen(command3)*sizeof(TCHAR));
  RegSetValueEx(key, TEXT("language"), 0, REG_DWORD, (const BYTE*)&language, sizeof(language));
  RegSetValueEx(key, TEXT("3way_compare_supported"), 0, REG_DWORD, (const BYTE*)&three_way_compare_supported_value, sizeof(three_way_compare_supported_value));
  RegCloseKey(key);
  
  if(compare_folders == BST_CHECKED) {
    LRESULT  result = NOERROR;
    DWORD disp;
    LPWSTR  tmp_guid;
    TCHAR class_id[MAX_PATH];
    
    if(StringFromIID(&CLSID_DIFF_EXT, &tmp_guid) == S_OK) {
      TCHAR value[MAX_PATH];
#ifdef UNICODE    
      _tcsncpy(class_id, tmp_guid, MAX_PATH);
#else
      wcstombs(class_id, tmp_guid, MAX_PATH);
#endif
      CoTaskMemFree((void*)tmp_guid);
    
      _stprintf(value, TEXT("%s"), class_id);
      
      result = RegCreateKeyEx(HKEY_CLASSES_ROOT, TEXT("Folder\\shellex\\ContextMenuHandlers\\diff-ext"), 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &key, &disp);
    
      if(result == NOERROR) {
        result = RegSetValueEx(key, 0, 0, REG_SZ, (LPBYTE)value, lstrlen(value)*sizeof(TCHAR));
        
        RegCloseKey(key);
      }
      
      result = RegCreateKeyEx(HKEY_CLASSES_ROOT, TEXT("Directory\\shellex\\ContextMenuHandlers\\diff-ext"), 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &key, &disp);
    
      if(result == NOERROR) {
        result = RegSetValueEx(key, 0, 0, REG_SZ, (LPBYTE)value, lstrlen(value)*sizeof(TCHAR));
        
        RegCloseKey(key);
      }
    }
  } else {
    RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("Folder\\shellex\\ContextMenuHandlers\\diff-ext"));
    RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("Directory\\shellex\\ContextMenuHandlers\\diff-ext"));
  }
}

static BOOL CALLBACK
main_dialog_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param) {
  BOOL ret = FALSE;

  switch(msg) {
    case WM_INITDIALOG:
      init(dialog, w_param, l_param);
      ret = TRUE;
      break;

    case WM_COMMAND:
      switch(LOWORD(w_param)) {
        case ID_COMMAND_DIFF3:
        case ID_DIFF_COMMAND: {
            static int first = 1;
            if((first != 0) && (HIWORD(w_param) == EN_SETFOCUS)) {
              SendDlgItemMessage(dialog, ID_DIFF_COMMAND, EM_SETSEL, 0, 0);
              SendDlgItemMessage(dialog, ID_COMMAND_DIFF3, EM_SETSEL, 0, 0);
	      
              SendDlgItemMessage(dialog, ID_DIFF_COMMAND, EM_SETMARGINS, EC_RIGHTMARGIN, MAKELPARAM(0, 3));
              SendDlgItemMessage(dialog, ID_COMMAND_DIFF3, EM_SETMARGINS, EC_RIGHTMARGIN, MAKELPARAM(0, 3));
	      
              first = 0;
            }
          }
          break;
          
	case ID_ABOUT:
	  about(resource, dialog);
	  break;
	
        case ID_BROWSE: {
            OPENFILENAME ofn;
            TCHAR szFile[MAX_PATH] = TEXT("");

            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = dialog;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile)/sizeof(szFile[0]);
            ofn.lpstrFilter = TEXT("Applications (*.exe)\0*.EXE\0All (*.*)\0*.*\0");
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
	    ofn.lpstrTitle = TEXT("Select file compare utility");
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;

            if(GetOpenFileName(&ofn) == TRUE) {
              SetDlgItemText(dialog, ID_DIFF_COMMAND, ofn.lpstrFile);
	    }

            ret = TRUE;
          }
          break;
          
        case ID_BROWSE1: {
            OPENFILENAME ofn;
            TCHAR szFile[MAX_PATH] = TEXT("");

            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = dialog;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile)/sizeof(szFile[0]);
            ofn.lpstrFilter = TEXT("Applications (*.exe)\0*.EXE\0All (*.*)\0*.*\0");
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
	    ofn.lpstrTitle = TEXT("Select file compare utility");
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;

            if(GetOpenFileName(&ofn) == TRUE) {
              SetDlgItemText(dialog, ID_COMMAND_DIFF3, ofn.lpstrFile);
	    }

            ret = TRUE;
          }
          break;
          
        case ID_APPLY:
        case IDOK: {
            HKEY key;
	    DWORD language = 1033;
	    DWORD old_language = 1033;
	    DWORD hlen;
            RECT rect;
            
            GetWindowRect(dialog, &rect);
            window_placement->x = rect.left;
            window_placement->y = rect.top;
            window_placement->width = rect.right-rect.left;
            window_placement->height = rect.bottom-rect.top;
          
	    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
	      hlen = sizeof(DWORD);
	      if(RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&old_language, &hlen) != ERROR_SUCCESS) {
		old_language = 1033;
	      }

	      RegCloseKey(key);
	    }
	    
	    apply(dialog);
	    
	    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
	      hlen = sizeof(DWORD);
	      if(RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&language, &hlen) != ERROR_SUCCESS) {
		language = 1033;
	      }

	      RegCloseKey(key);
	    }
	    
	    if((LOWORD(w_param) == IDOK) || (language != old_language)) {
	      EndDialog(dialog, LOWORD(w_param));
	    }
	    
            ret = TRUE;
          }
          break;

        case IDCANCEL:
          EndDialog(dialog, IDCANCEL);
          ret = TRUE;
          break;
      }
      break;

    case WM_MOVE: {
        RECT rect;
        
        GetWindowRect(dialog, &rect);
        window_placement->x = rect.left;
        window_placement->y = rect.top;
        window_placement->width = rect.right-rect.left;
        window_placement->height = rect.bottom-rect.top;
      }
      break;
      
      case WM_CLOSE:
      EndDialog(dialog,0);
      ret = TRUE;
      break;
  }

  return ret;
}
