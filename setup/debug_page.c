/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#include <windows.h>
#include <commctrl.h>

#include "layout.h"
#include "page.h"

#include "diff_ext_setup.rh"

typedef struct {
  PAGE super;
  int enabled;
  TCHAR log_path[MAX_PATH];
} DEBUG_PAGE;

static BOOL CALLBACK debug_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param);

static void
apply(PAGE* page) {
  HKEY key;
  TCHAR log_path[MAX_PATH];
  LRESULT enabled = 0;
  
  GetDlgItemText(page->page, ID_LOG_PATH, log_path, MAX_PATH);
  if(IsDlgButtonChecked(page->page, ID_ENABLE_LOGGING)) {
    enabled = 1;
  }

  RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_SET_VALUE, &key);
  RegSetValueEx(key, TEXT("log_file"), 0, REG_SZ, (const BYTE*)log_path, lstrlen(log_path));
  RegSetValueEx(key, TEXT("log"), 0, REG_DWORD, (const BYTE*)&enabled, sizeof(enabled));
  RegCloseKey(key);
}

PAGE* 
create_debug_page(HANDLE resource, HWND parent) {
  PAGE* page = (PAGE*)malloc(sizeof(PAGE));
  
  if(page != 0) {
    HGLOBAL dialog_handle;
    HRSRC resource_handle;
    DLGTEMPLATE* dialog_template;
    LAYOUT* layout;
    WINDOW_DATA* data = (WINDOW_DATA*)malloc(sizeof(WINDOW_DATA));
    
    page->apply = apply;
    
    layout = create_layout(resource, MAKEINTRESOURCE(IDD_LOGGING), MAKEINTRESOURCE(ID_LOGGING_LAYOUT));
    
    resource_handle = FindResource(resource, MAKEINTRESOURCE(IDD_LOGGING), RT_DIALOG);
    dialog_handle = LoadResource(resource, resource_handle);
    dialog_template = (DLGTEMPLATE*)LockResource(dialog_handle);
  
    memcpy(&(data->layout), layout, sizeof(LAYOUT));
    data->page = page;
    free(layout);
    page->page = CreateDialogIndirectParam(resource, dialog_template, parent, (DLGPROC)debug_func, (LPARAM)data);
  }
  
  return page;
}

static void
enable_logging(HWND dialog) {
  HWND item;
  
  item = GetDlgItem(dialog, ID_BROWSE);
  EnableWindow(item, TRUE);
  item = GetDlgItem(dialog, ID_LOG_PATH);
  EnableWindow(item, TRUE);
  item = GetDlgItem(dialog, ID_LOG_LEVEL_LABEL);
  EnableWindow(item, TRUE);
  item = GetDlgItem(dialog, ID_LOG_LEVEL_BUTTON);
  EnableWindow(item, TRUE);
  item = GetDlgItem(dialog, ID_LOG_LEVEL);
  EnableWindow(item, TRUE);
}

static void
disable_logging(HWND dialog) {
  HWND item;
  
  item = GetDlgItem(dialog, ID_BROWSE);
  EnableWindow(item, FALSE);
  item = GetDlgItem(dialog, ID_LOG_PATH);
  EnableWindow(item, FALSE);
  item = GetDlgItem(dialog, ID_LOG_LEVEL_LABEL);
  EnableWindow(item, FALSE);
  item = GetDlgItem(dialog, ID_LOG_LEVEL_BUTTON);
  EnableWindow(item, FALSE);
  item = GetDlgItem(dialog, ID_LOG_LEVEL);
  EnableWindow(item, FALSE);
}

static void
init(HWND dialog, WPARAM not_used, LPARAM l_param) {
  HWND log_level_button = GetDlgItem(dialog, ID_LOG_LEVEL_BUTTON);
  HKEY key;
  TCHAR log_path[MAX_PATH] = TEXT("");
  LRESULT enabled = 0;
    
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
    DWORD hlen = MAX_PATH;
  
    RegQueryValueEx(key, TEXT("log_file"), 0, NULL, (BYTE*)log_path, &hlen);

    hlen = sizeof(DWORD);
    if(RegQueryValueEx(key, TEXT("log"), 0, NULL, (BYTE*)(&enabled), &hlen) != ERROR_SUCCESS) {
      enabled = 0;
    }

    RegCloseKey(key);
  }
  
  if(enabled == 1) {
    CheckDlgButton(dialog, ID_ENABLE_LOGGING, BST_CHECKED);
    enable_logging(dialog);
  } else {
    CheckDlgButton(dialog, ID_ENABLE_LOGGING, BST_UNCHECKED);
    disable_logging(dialog);
  }
  
  SetDlgItemText(dialog, ID_LOG_PATH, log_path);
  SendMessage(log_level_button, UDM_SETRANGE, 0, (LPARAM) MAKELONG (20, 0));
  
  SetWindowLongPtr(dialog, DWLP_USER, l_param);
}

static BOOL CALLBACK
debug_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param) {
  BOOL ret = FALSE;

  switch (msg) {
    case WM_INITDIALOG:
      init(dialog, w_param, l_param);
      ret = TRUE;
      break;
    
    case WM_COMMAND:
      switch(LOWORD(w_param)) {
	case ID_ENABLE_LOGGING:
	  if(IsDlgButtonChecked(dialog, ID_ENABLE_LOGGING)) {
	    enable_logging(dialog);
	  } else {
	    disable_logging(dialog);
	  }
	  break;

        case ID_BROWSE: {
            OPENFILENAME ofn;
            TCHAR szFile[MAX_PATH] = "";

            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = dialog;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile)/sizeof(szFile[0]);
            ofn.lpstrFilter = TEXT("Log files (*.log)\0*.LOG\0All (*.*)\0*.*\0");
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
	    ofn.lpstrTitle = TEXT("Select log file");
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;

            if(GetOpenFileName(&ofn) == TRUE) {
              SetDlgItemText(dialog, ID_LOG_PATH, ofn.lpstrFile);
	    }
          }
          break;
      }
      ret = TRUE;
      break;
  }
  
  return ret;
}
  
