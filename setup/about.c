/*
 * Copyright (c) 2003-2006, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#include <windows.h>

#include "diff_ext_setup.h"

static BOOL CALLBACK about_dialog_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param);

void
about(HANDLE resource, HWND parent) {
  DialogBoxParam(resource, MAKEINTRESOURCE(IDD_ABOUT), parent, about_dialog_func, (LPARAM)resource);
}

static void
init(HWND dialog, WPARAM not_used_1, LPARAM l_param) {
  DWORD file_versioninfo_size;
  DWORD version_handle;
  TCHAR path[MAX_PATH];
  HANDLE resource = (HANDLE)l_param;
  
  GetModuleFileName(resource, path, sizeof(path)/sizeof(path[0]));
  file_versioninfo_size = GetFileVersionInfoSize(path, &version_handle);
  
  if(file_versioninfo_size > 0) {
    void* file_versioninfo = malloc(file_versioninfo_size);
    struct {WORD language; WORD codepage;}* translations;
    UINT length = 0;
    TCHAR version_block[] = TEXT("\\StringFileInfo\\12341234\\ProductVersion");
    TCHAR* product_version;
    
    GetFileVersionInfo(path, 0, file_versioninfo_size, file_versioninfo);
    
    VerQueryValue(file_versioninfo, TEXT("\\VarFileInfo\\Translation"), (void**)&translations, &length);
    
    if(length > 0) {
      wsprintf(version_block, TEXT("\\StringFileInfo\\%04x%04x\\ProductVersion"), translations[0].language, translations[0].codepage);
      
      VerQueryValue(file_versioninfo, version_block, (void**)&product_version, &length);
      
      if(length > 0) {
	SetDlgItemText(dialog, ID_VERSION, product_version);
      }
    }
    
    free(file_versioninfo);
  }
}

static BOOL CALLBACK
about_dialog_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param) {
  BOOL ret = FALSE;

  switch (msg) {
    case WM_INITDIALOG:
      init(dialog, w_param, l_param);
      ret = TRUE;
      break;
    
    case WM_CLOSE:
      EndDialog(dialog, 1);
      ret = TRUE;
      break;
    
    case WM_COMMAND:
      if(LOWORD(w_param) == IDOK) {
	EndDialog(dialog, 1);
	ret = TRUE;
      }
      break;
  }
  
  return ret;
}

