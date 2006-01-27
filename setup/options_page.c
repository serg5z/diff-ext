/*
 * Copyright (c) 2003-2005, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
 #define INITGUID
 #include <stdio.h>
 #include <windows.h>
 #include <tchar.h>

#include <shlguid.h>
#include <olectl.h>
#include <objidl.h>

#include <objbase.h>
#include <initguid.h>

#include "layout.h"
#include "page.h"

#include "diff_ext_setup.rh"

typedef struct {
  PAGE super;
  LRESULT language;
  TCHAR command[MAX_PATH];
} OPTIONS_PAGE;

static BOOL CALLBACK options_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param);

DEFINE_GUID(CLSID_DIFF_EXT, 0xA0482097, 0xC69D, 0x4DEC, 0x8A, 0xB6, 0xD3, 0xA2, 0x59, 0xAC, 0xC1, 0x51);

static void
apply(PAGE* page) {
  HKEY key;
  TCHAR command[MAX_PATH];
  LRESULT language;
  LRESULT idx;
  LRESULT compare_folders;
  
  GetDlgItemText(page->page, ID_DIFF_COMMAND, command, MAX_PATH);
  idx = SendDlgItemMessage(page->page, ID_LANGUAGE, CB_GETCURSEL, 0, 0);
  language = SendDlgItemMessage(page->page, ID_LANGUAGE, CB_GETITEMDATA, idx, 0);

  RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff-ext\\"), 0, KEY_SET_VALUE, &key);
  RegSetValueEx(key, TEXT("diff"), 0, REG_SZ, (const BYTE*)command, _tcslen(command)*sizeof(TCHAR));
  RegSetValueEx(key, TEXT("language"), 0, REG_DWORD, (const BYTE*)&language, sizeof(language));
  RegCloseKey(key);
  
  compare_folders = SendDlgItemMessage(page->page, ID_DIFF_DIRS, BM_GETCHECK, 0, 0);
  
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
        result = RegSetValueEx(key, 0, 0, REG_SZ, (LPBYTE)value, _tcslen(value)*sizeof(TCHAR));
        
        RegCloseKey(key);
      }
      
      result = RegCreateKeyEx(HKEY_CLASSES_ROOT, TEXT("Directory\\shellex\\ContextMenuHandlers\\diff-ext"), 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &key, &disp);
    
      if(result == NOERROR) {
        result = RegSetValueEx(key, 0, 0, REG_SZ, (LPBYTE)value, _tcslen(value)*sizeof(TCHAR));
        
        RegCloseKey(key);
      }
    }
  } else {
    RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("Folder\\shellex\\ContextMenuHandlers\\diff-ext"));
    RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("Directory\\shellex\\ContextMenuHandlers\\diff-ext"));
  }
}

PAGE* 
create_options_page(HANDLE resource, HWND parent) {
  PAGE* page = (PAGE*)malloc(sizeof(PAGE));
  
  if(page != 0) {
    HGLOBAL dialog_handle;
    HRSRC resource_handle;
    DLGTEMPLATE* dialog_template;
    
    page->apply = apply;
    
/*    layout = create_layout(resource, MAKEINTRESOURCE(IDD_OPTIONS), MAKEINTRESOURCE(ID_OPTIONS_LAYOUT));*/
    
    resource_handle = FindResource(resource, MAKEINTRESOURCE(IDD_OPTIONS), RT_DIALOG);
    dialog_handle = LoadResource(resource, resource_handle);
    dialog_template = (DLGTEMPLATE*)LockResource(dialog_handle);
  
    page->page = CreateDialogIndirect(resource, dialog_template, parent, (DLGPROC)options_func);

    attach_layout(resource, page->page, MAKEINTRESOURCE(ID_OPTIONS_LAYOUT));
  }
  
  return (PAGE*)page;
}

static void
init(HWND dialog, WPARAM not_used, LPARAM l_param) {
  HKEY key;
  TCHAR command[MAX_PATH] = TEXT("");
  TCHAR home[MAX_PATH] = TEXT(".");
  DWORD language = 1033;
  HANDLE file;
  WIN32_FIND_DATA file_info;
  TCHAR prefix[] = TEXT("diff_ext_setup");
  TCHAR root[] = TEXT("????");
  TCHAR suffix[] = TEXT(".dll");
  TCHAR* locale_info;
  int locale_info_size;
  int curr = 0;  
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
    
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("Folder\\shellex\\ContextMenuHandlers\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
      DWORD hlen = MAX_PATH;
    
      RegQueryValueEx(key, TEXT(""), 0, NULL, (BYTE*)clsid, &hlen);
      
      if(_tcsncmp(clsid, class_id, MAX_PATH) == 0) {
        SendDlgItemMessage(dialog, ID_DIFF_DIRS, BM_SETCHECK, BST_CHECKED, 0);
      }
      
      RegCloseKey(key);
    }
    
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("Directory\\shellex\\ContextMenuHandlers\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
      DWORD hlen = MAX_PATH;
    
      RegQueryValueEx(key, TEXT(""), 0, NULL, (BYTE*)clsid, &hlen);
      
      if(_tcsncmp(clsid, class_id, MAX_PATH) == 0) {
        SendDlgItemMessage(dialog, ID_DIFF_DIRS, BM_SETCHECK, BST_CHECKED, 0);
      }
      
      RegCloseKey(key);
    }
  }
  
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
    DWORD hlen = MAX_PATH;
  
    RegQueryValueEx(key, TEXT("diff"), 0, NULL, (BYTE*)command, &hlen);

    hlen = sizeof(DWORD);
    if(RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)(&language), &hlen) != ERROR_SUCCESS) {
      language = 1033;
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
      
      _stscanf(file_info.cFileName, TEXT("diff_ext_setup%4u.dll"), &lang_id);

      locale_info_size = GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, 0, 0);
      locale_info = (TCHAR*)malloc(locale_info_size*sizeof(TCHAR));
      GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

      curr = SendDlgItemMessage(dialog, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
      SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETITEMDATA, curr, lang_id);

      if(lang_id == language) {
	SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETCURSEL, curr, 0);
      }

      free(locale_info);

      stop = !FindNextFile(file, &file_info);
    }

    FindClose(file);
  }
  
  SetDlgItemText(dialog, ID_DIFF_COMMAND, command);
  SendDlgItemMessage(dialog, ID_DIFF_COMMAND, EM_SETLIMITTEXT, MAX_PATH, 0);
}

static BOOL CALLBACK
options_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param) {
  BOOL ret = FALSE;

  switch (msg) {
    case WM_INITDIALOG:
      init(dialog, w_param, l_param);
      ret = TRUE;
      break;  
    
    case WM_COMMAND:
      switch(LOWORD(w_param)) {
        case ID_DIFF_COMMAND: {
            static int first = 1;
            if((first != 0) && (HIWORD(w_param) == EN_SETFOCUS)) {
              SendDlgItemMessage(dialog, ID_DIFF_COMMAND, EM_SETSEL, 0, 0);
/*	      
              SendDlgItemMessage(dialog, ID_DIFF_COMMAND, EM_SETMARGINS, EC_RIGHTMARGIN, MAKELPARAM(0, 30));
*/	      
              first = 0;
            }
          }
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
	}
      break;
  }
  
  return ret;
}
  
