/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>

#include "layout.h"

#include "diff_ext_setup_res.h"

typedef struct  {
  int x;
  int y;
  int w;
  int h;
} WINDOW_POSITION;

WINDOW_POSITION* last_position = 0;

static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY
WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc;
  HRESULT exit = ID_APPLY;
  HKEY key;
  DWORD language = 0;
  DWORD hlen;
  char language_lib[MAX_PATH];
  HGLOBAL dialog_handle;
  HRSRC resource_handle;
  DLGTEMPLATE* dialog;
  HANDLE resource;
  LAYOUT* layout;

  while(exit == ID_APPLY) {
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
      hlen = sizeof(DWORD);
      RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&language, &hlen);
  
      RegCloseKey(key);
  
      sprintf(language_lib, "diff_ext_setup%ld.dll", language);
    }
  
    resource = LoadLibrary(language_lib);
  
    if(resource == NULL)
      resource = hinst;
  
    memset(&wc,0,sizeof(wc));
    wc.lpfnWndProc = DefDlgProc;
    wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = hinst;
    wc.hIcon = LoadIcon(resource, MAKEINTRESOURCE(MAIN_ICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszClassName = "diff-ext-setup";
    UnregisterClass(wc.lpszClassName, hinst);
    RegisterClass(&wc);
  
    SetThreadLocale(LOCALE_USER_DEFAULT);
    InitCommonControls();
  
    resource_handle = FindResource(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), RT_DIALOG);
    dialog_handle = LoadResource(resource, resource_handle);
    dialog = (DLGTEMPLATE*)LockResource(dialog_handle);
  
    layout = create_layout(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), MAKEINTRESOURCE(ID_MAINDIALOG_LAYOUT));
  /*  can not do because have to specify hinst as module instance and load dialog from translated resource only dll...
    ret = DialogBox(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)DialogFunc);
  */  
    exit = DialogBoxIndirectParam(hinst, dialog, NULL, (DLGPROC)DialogFunc, (LPARAM)layout);
  
    FreeResource(dialog);
    FreeLibrary(resource);
    free(layout);
  }
  
  return exit;
}

static void
InitializeApp(HWND hDlg,WPARAM wParam, LPARAM lParam) {
  HKEY key;
  TCHAR command[MAX_PATH] = TEXT("");
  TCHAR home[MAX_PATH] = TEXT(".");
  DWORD hlen;
  HANDLE file;
  WIN32_FIND_DATA file_info;
  DWORD language = 1033;
  TCHAR prefix[] = TEXT("diff_ext_setup");
  TCHAR root[] = TEXT("????");
  TCHAR suffix[] = TEXT(".dll");
  TCHAR* locale_info;
  int locale_info_size;
  int curr = 0;

  ZeroMemory(command, MAX_PATH);

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {

    hlen = MAX_PATH;
    RegQueryValueEx(key, TEXT("diff"), 0, NULL, (BYTE*)command, &hlen);

    hlen = sizeof(DWORD);
    if(RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&language, &hlen) != ERROR_SUCCESS) {
      language = 1033;
    }

    hlen = MAX_PATH;
    if(RegQueryValueEx(key, TEXT("home"), 0, NULL, (BYTE*)home, &hlen) != ERROR_SUCCESS) {
      lstrcpy(home, TEXT("."));
    }

    RegCloseKey(key);
  }
  
  locale_info_size = GetLocaleInfo(1033, LOCALE_SNATIVELANGNAME, 0, 0);
  locale_info = (TCHAR*)malloc(locale_info_size);
  GetLocaleInfo(1033, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

  SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
  SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_SETITEMDATA, curr, 1033);

  if(language == 1033)
    SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_SETCURSEL, curr, 0);

  free(locale_info);
  curr++;

  lstrcat(home, "\\");
  lstrcat(home, prefix);
  lstrcat(home, root);
  lstrcat(home, suffix);

  file = FindFirstFile(home, &file_info);
  if(file != INVALID_HANDLE_VALUE) {
    BOOL stop = FALSE;

    while(stop == FALSE) {
      char* str = file_info.cFileName+sizeof(prefix)/sizeof(char)-1;
      DWORD lang_id;

      str[sizeof(root)/sizeof(char)-1] = 0;

      lang_id = atoi(str);

      locale_info_size = GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, 0, 0);
      locale_info = (TCHAR*)malloc(locale_info_size);
      GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

      SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
      SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_SETITEMDATA, curr, lang_id);

      if(lang_id == language)
        SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_SETCURSEL, curr, 0);

      free(locale_info);

      stop = !FindNextFile(file, &file_info);
      curr++;
    }

    FindClose(file);
  }

  SetDlgItemText(hDlg, ID_DIFF_COMMAND, command);
  SendDlgItemMessage(hDlg, ID_DIFF_COMMAND, EM_SETLIMITTEXT, MAX_PATH, 0);
  
  SetWindowLongPtr(hDlg, DWLP_USER, lParam);
  
  if(last_position != 0) {
    MoveWindow(hDlg, last_position->x, last_position->y, last_position->w, last_position->h, TRUE);
  }
}

static BOOL CALLBACK
DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
  BOOL ret = FALSE;

  switch (msg) {
    case WM_INITDIALOG:
      InitializeApp(hwndDlg,wParam,lParam);
      ret = TRUE;
      break;

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case ID_DIFF_COMMAND: {
            static int first = 1;
            if((first != 0) && (HIWORD(wParam) == EN_SETFOCUS)) {
              SendDlgItemMessage(hwndDlg, ID_DIFF_COMMAND, EM_SETSEL, 0, 0);
              SendDlgItemMessage(hwndDlg, ID_DIFF_COMMAND, EM_SETMARGINS, EC_RIGHTMARGIN, MAKELPARAM(0, 30));
              first = 0;
            }
          }
          break;

        case ID_APPLY:
        case IDOK: {
            HKEY key;
            TCHAR command[MAX_PATH];
	    LRESULT language;
	    LRESULT old_language;
	    LRESULT idx;
	    DWORD hlen;

	    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
	      hlen = sizeof(DWORD);
	      if(RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&old_language, &hlen) != ERROR_SUCCESS) {
		old_language = 1033;
	      }

	      RegCloseKey(key);
	    }
	    
            GetDlgItemText(hwndDlg, ID_DIFF_COMMAND, command, MAX_PATH);
	    idx = SendDlgItemMessage(hwndDlg, ID_LANGUAGE, CB_GETCURSEL, 0, 0);
	    language = SendDlgItemMessage(hwndDlg, ID_LANGUAGE, CB_GETITEMDATA, idx, 0);

            RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_SET_VALUE, &key);
            RegSetValueEx(key, TEXT("diff"), 0, REG_SZ, (const BYTE*)command, strlen(command));
            RegSetValueEx(key, TEXT("language"), 0, REG_DWORD, (const BYTE*)&language, sizeof(language));
            RegCloseKey(key);

	    if((LOWORD(wParam) == IDOK) || (language != old_language)) {
	      EndDialog(hwndDlg,LOWORD(wParam));
	    }
	    
            ret = TRUE;
          }
          break;

        case IDCANCEL:
          EndDialog(hwndDlg,IDCANCEL);
          ret = TRUE;
          break;

        case ID_BROWSE: {
            OPENFILENAME ofn;
            TCHAR szFile[MAX_PATH] = "";
            DWORD err;

            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hwndDlg;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile)/sizeof(szFile[0]);
            ofn.lpstrFilter = "Applications (*.exe)\0*.EXE\0All (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
	    ofn.lpstrTitle = "Select file compare utility";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

            if(GetOpenFileName(&ofn) == TRUE)
              SetDlgItemText(hwndDlg, ID_DIFF_COMMAND, ofn.lpstrFile);

            err = CommDlgExtendedError();

            ret = TRUE;
          }
          break;
      }
      break;

    case WM_GETMINMAXINFO: {
        LAYOUT* layout = (LAYOUT*)GetWindowLongPtr(hwndDlg, DWLP_USER);
        MINMAXINFO* min_max_info = (MINMAXINFO*)lParam;
        RECT client;
        WINDOWINFO info;

        client.top = 0;
        client.bottom = layout->height;
        client.left = 0;
        client.right = layout->width;

        MapDialogRect(hwndDlg, &client);
        GetWindowInfo(hwndDlg, &info);
        AdjustWindowRectEx(&client, info.dwStyle, FALSE, info.dwExStyle);

        min_max_info->ptMinTrackSize.x = client.right-client.left;
        min_max_info->ptMinTrackSize.y = client.bottom-client.top;
        ret = TRUE;
      }
      break;

    case WM_MOVE: {
        RECT rect;
        if(last_position == 0) {
	  last_position = (WINDOW_POSITION*)malloc(sizeof(WINDOW_POSITION));
	}
	
	GetWindowRect(hwndDlg, &rect);
	
	last_position->x = rect.left;
	last_position->y = rect.top;
	last_position->w = rect.right - rect.left;
	last_position->h = rect.bottom - rect.top;

        ret = TRUE;
      }
      break;
      
    case WM_SIZE: {
        RECT rect;
/**/
        layout(hwndDlg);
/* redraw them all*/
        InvalidateRect(hwndDlg, 0, TRUE);
        UpdateWindow(hwndDlg);
      
        if(last_position == 0) {
	  last_position = (WINDOW_POSITION*)malloc(sizeof(WINDOW_POSITION));
	}
	
	GetWindowRect(hwndDlg, &rect);
	
	last_position->x = rect.left;
	last_position->y = rect.top;
	last_position->w = rect.right - rect.left;
	last_position->h = rect.bottom - rect.top;

        ret = TRUE;
      }
      break;

    case WM_CLOSE:
      EndDialog(hwndDlg,0);
      ret = TRUE;
      break;
  }

  return ret;
}
