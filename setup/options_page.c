/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#include "layout.h"
#include "page.h"

#include "diff_ext_setup.rh"

typedef struct {
  PAGE super;
  LRESULT language;
  TCHAR command[MAX_PATH];
} OPTIONS_PAGE;

static BOOL CALLBACK options_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param);

static void
apply(PAGE* page) {
  HKEY key;
  TCHAR command[MAX_PATH];
  LRESULT language;
  LRESULT idx;
  
  GetDlgItemText(page->page, ID_DIFF_COMMAND, command, MAX_PATH);
  idx = SendDlgItemMessage(page->page, ID_LANGUAGE, CB_GETCURSEL, 0, 0);
  language = SendDlgItemMessage(page->page, ID_LANGUAGE, CB_GETITEMDATA, idx, 0);

  RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_SET_VALUE, &key);
  RegSetValueEx(key, TEXT("diff"), 0, REG_SZ, (const BYTE*)command, lstrlen(command));
  RegSetValueEx(key, TEXT("language"), 0, REG_DWORD, (const BYTE*)&language, sizeof(language));
  RegCloseKey(key);
}

PAGE* 
create_options_page(HANDLE resource, HWND parent) {
  PAGE* page = (PAGE*)malloc(sizeof(PAGE));
  
  if(page != 0) {
    HGLOBAL dialog_handle;
    HRSRC resource_handle;
    DLGTEMPLATE* dialog_template;
    LAYOUT* layout;
    WINDOW_DATA* data = (WINDOW_DATA*)malloc(sizeof(WINDOW_DATA));
    
    page->apply = apply;
    
    layout = create_layout(resource, MAKEINTRESOURCE(IDD_OPTIONS), MAKEINTRESOURCE(ID_OPTIONS_LAYOUT));
    
    resource_handle = FindResource(resource, MAKEINTRESOURCE(IDD_OPTIONS), RT_DIALOG);
    dialog_handle = LoadResource(resource, resource_handle);
    dialog_template = (DLGTEMPLATE*)LockResource(dialog_handle);
  
    memcpy(&(data->layout), layout, sizeof(LAYOUT));
    data->page = page;
    free(layout);
    page->page = CreateDialogIndirectParam(resource, dialog_template, parent, (DLGPROC)options_func, (LPARAM)data);
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
  
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
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
  locale_info = (TCHAR*)malloc(locale_info_size);
  GetLocaleInfo(1033, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

  SendDlgItemMessage(dialog, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
  SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETITEMDATA, curr, 1033);

  if(language == 1033) {
    SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETCURSEL, curr, 0);
  }

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
      TCHAR* str = file_info.cFileName+sizeof(prefix)/sizeof(prefix[0])-1;
      DWORD lang_id;

      str[sizeof(root)/sizeof(root[0])-1] = 0;

#ifdef UNICODE
      lang_id = _wtoi(str);
#else
	  lang_id = atoi(str);
#endif

      locale_info_size = GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, 0, 0);
      locale_info = (TCHAR*)malloc(locale_info_size);
      GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

      SendDlgItemMessage(dialog, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
      SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETITEMDATA, curr, lang_id);

      if(lang_id == language) {
	SendDlgItemMessage(dialog, ID_LANGUAGE, CB_SETCURSEL, curr, 0);
      }

      free(locale_info);

      stop = !FindNextFile(file, &file_info);
      curr++;
    }

    FindClose(file);
  }

  SetDlgItemText(dialog, ID_DIFF_COMMAND, command);
  SendDlgItemMessage(dialog, ID_DIFF_COMMAND, EM_SETLIMITTEXT, MAX_PATH, 0);

  SetWindowLongPtr(dialog, DWLP_USER, l_param);
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
  
