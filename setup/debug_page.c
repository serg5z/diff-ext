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
  int enabled;
  TCHAR log_path[MAX_PATH];
} DEBUG_PAGE;

static BOOL CALLBACK debug_func(HWND dialog, UINT msg, WPARAM w_param, LPARAM l_param);

static void
apply(PAGE* page) {
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
    
    layout = create_layout(resource, MAKEINTRESOURCE(IDD_DEBUG), MAKEINTRESOURCE(ID_DEBUG_LAYOUT));
    
    resource_handle = FindResource(resource, MAKEINTRESOURCE(IDD_DEBUG), RT_DIALOG);
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
init(HWND dialog, WPARAM not_used, LPARAM l_param) {
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
  }
  
  return ret;
}
  
