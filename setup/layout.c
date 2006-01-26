/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#include <stdio.h>
#include "layout.h"

static const DWORD ANCOR_LEFT  =0x000001;
static const DWORD ANCOR_TOP   =0x000002;
static const DWORD ANCOR_RIGHT =0x000004;
static const DWORD ANCOR_BOTTOM=0x000008;

typedef struct {
  WORD version;
  WORD signature;
  DWORD help_iD;
  DWORD extended_style;
  DWORD style;
  WORD cdit;
  short x;
  short y;
  short cx;
  short cy;
} DLGTEMPLATEEX;

typedef struct {
  DWORD helpID;
  DWORD exStyle;
  DWORD style;
  short x;
  short y;
  short cx;
  short cy;
  DWORD id;
} DLGITEMTEMPLATEEX;

typedef struct {
  DWORD id;
  DWORD top_left_anchor;
  DWORD bottom_right_anchor;
} LAYOUT_ITEM_RC;

typedef struct {
  int x;
  int y;
  DWORD anchor;
} LAYOUT_COORD;

typedef struct {
  DWORD id;
  LAYOUT_COORD top_left;
  LAYOUT_COORD bottom_right;
} LAYOUT_ITEM;

typedef struct LAYOUT_ITEM_LIST {
  LAYOUT_ITEM item;
  struct LAYOUT_ITEM_LIST* next;
} LAYOUT_ITEM_LIST;

static BOOL
is_extended_dialog(DLGTEMPLATE* tpl) {
  return (HIWORD(tpl->style) == 0xffff);
}

static DLGITEMTEMPLATE*
first_item(DLGTEMPLATE* tpl) {
  WORD* current;
  DWORD style;
  
  if(is_extended_dialog(tpl)) {
    DLGTEMPLATEEX* tplex = (DLGTEMPLATEEX*)tpl;
    style = tplex->style;
    current = (WORD*)(tplex+1);
  }
  else {
    style = tpl->style;
    current = (WORD*)(tpl+1);
  }
  
/* skip menu */  
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;
  
/* skip class */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;

/* skip title */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;
  
  if (style & DS_SETFONT) {
    current++;
    while((*current) != 0) {
      current++;
    }
    current++;
  }
  
  return (DLGITEMTEMPLATE*)(((DWORD)current+3) & ~(DWORD)3 );
}

static DLGITEMTEMPLATE*
next_item(DLGITEMTEMPLATE* tpl) {
  WORD* current;
  WORD extra_size;
  
  tpl++;
  current = (WORD*)tpl;
  
/* skip class */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;

/* skip text */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;
  
/* testme!!! */  
  extra_size = *current;
  if(extra_size > 0) {
    current = (WORD*)((BYTE*)current + extra_size);
  }
  else {
    current++;
  }
  
  return (DLGITEMTEMPLATE*)(((DWORD)current+3) & ~(DWORD)3);
}

static DLGITEMTEMPLATEEX*
next_extended_item(DLGITEMTEMPLATEEX* tpl) {
  WORD* current;
  WORD extra_size;
  
  tpl++;
  current = (WORD*)tpl;
  
/* skip class */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;

/* skip text */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;
  
/* testme!!! */  
  extra_size = *current;
  if(extra_size > 0) {
    current = (WORD*)((BYTE*)current + extra_size);
  }
  else {
    current++;
  }
  
  return (DLGITEMTEMPLATEEX*)(((DWORD)current+3) & ~(DWORD)3);
}

static LRESULT
dialog_procedure(HWND dialog, UINT message, WPARAM w_param, LPARAM l_param) {
  LRESULT result = 0;
  
  if(message == WM_GETMINMAXINFO) {
    LAYOUT* layout = (LAYOUT*)GetWindowLongPtr(dialog, DWLP_USER);
    MINMAXINFO* min_max_info = (MINMAXINFO*)l_param;
    RECT client;
    WINDOWINFO info;

    if(layout != 0) {
      client.top = 0;
      client.bottom = layout->height;
      client.left = 0;
      client.right = layout->width;
    } else {
      GetClientRect(dialog, &client);
    }

/*    MapDialogRect(dialog, &client);*/
    GetWindowInfo(dialog, &info);
/*    AdjustWindowRectEx(&client, info.dwStyle, FALSE, info.dwExStyle);*/

    min_max_info->ptMinTrackSize.x = client.right-client.left;
    min_max_info->ptMinTrackSize.y = client.bottom-client.top;
  } else {
    if(message == WM_SIZE) {
      layout(dialog);
      /*InvalidateRect(dialog, 0, TRUE);*/
    }
    
    result = DefDlgProc(dialog, message, w_param, l_param);
  }

  return result;
}

static void
dump_layout(LAYOUT* layout) {
  LAYOUT_ITEM_LIST* current = (LAYOUT_ITEM_LIST*)layout->control_layout;
  
  _tprintf(TEXT("layout: %dx%d\n"), layout->width, layout->height);
  while(current != 0) {
    _tprintf(TEXT("control: %d\n"), current->item.id);
    _tprintf(TEXT("\ttop left: (%d, %d)\n"), current->item.top_left.x, current->item.top_left.y);
    _tprintf(TEXT("\tbottom right: (%d, %d)\n"), current->item.bottom_right.x, current->item.bottom_right.y);
    current = current->next;
  }
  
  _tprintf(TEXT("---------------------------\n"));
  
  fflush(stdout);
}

struct LAYOUT_CHILD_PARAM {
  LAYOUT* layout;
  LAYOUT_ITEM_RC* layout_table;
  unsigned int n;
  RECT dialog_rect;
};

static BOOL CALLBACK 
layout_child(HWND control, LPARAM l_param) {
  struct LAYOUT_CHILD_PARAM* param = (struct ENUM_PROC_PARAM*)l_param;
  DWORD item_id = GetDlgCtrlID(control);
  RECT control_rect;
  LAYOUT_ITEM_LIST* prev;
  int dialog_width = param->dialog_rect.right - param->dialog_rect.left;
  int dialog_height = param->dialog_rect.bottom - param->dialog_rect.top;
  int i;
  
  GetWindowRect(control, &control_rect);

  prev  = param->layout->control_layout;
  
  for(i = 0; i < param->n; i++) {
    if(param->layout_table[i].id == item_id) {
      LAYOUT_ITEM_LIST* item = (LAYOUT_ITEM_LIST*)malloc(sizeof(LAYOUT_ITEM_LIST));
      
      item->item.id = item_id;
      item->item.top_left.anchor = param->layout_table[i].top_left_anchor;
      item->item.bottom_right.anchor = param->layout_table[i].bottom_right_anchor;
      item->next = param->layout->control_layout;

      if(item->item.top_left.anchor & ANCOR_RIGHT) {
        item->item.top_left.x = control_rect.left - param->dialog_rect.left - dialog_width;
      }
  
      if(item->item.top_left.anchor & ANCOR_LEFT) {
        item->item.top_left.x = control_rect.left - param->dialog_rect.left;
      }
  
      if(item->item.top_left.anchor & ANCOR_TOP) {
        item->item.top_left.y = control_rect.top - param->dialog_rect.top;
      }
  
      if(item->item.top_left.anchor & ANCOR_BOTTOM) {
        item->item.top_left.y = control_rect.top - param->dialog_rect.top - dialog_height;
      }
  
      if(item->item.bottom_right.anchor & ANCOR_RIGHT) {
        item->item.bottom_right.x = control_rect.right - param->dialog_rect.left - dialog_width;
      }
  
      if(item->item.bottom_right.anchor & ANCOR_LEFT) {
        item->item.bottom_right.x = control_rect.right - param->dialog_rect.left;
      }
  
      if(item->item.bottom_right.anchor & ANCOR_TOP) {
        item->item.bottom_right.y = control_rect.bottom - param->dialog_rect.top;
      }
  
      if(item->item.bottom_right.anchor & ANCOR_BOTTOM) {
        item->item.bottom_right.y = control_rect.bottom - param->dialog_rect.top - dialog_height;
      }
  
      param->layout->control_layout = item;
    }
  }
  
  return TRUE;
}

void
init_layout() {
  WNDCLASS wc;
  HINSTANCE instance;
  
  instance = GetModuleHandle(0);

  memset(&wc,0,sizeof(wc));
  wc.lpfnWndProc = dialog_procedure;
  wc.cbWndExtra = DLGWINDOWEXTRA;
  wc.hInstance = instance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wc.lpszClassName = DIALOG_WITH_LAYOUT_CLASS;
  UnregisterClass(wc.lpszClassName, instance);
  RegisterClass(&wc);
}

LAYOUT* 
create_layout2(HANDLE resource, HWND dialog, LPCTSTR layout_name) {
  HGLOBAL layout_table_handle;
  HRSRC resource_handle;
  DWORD layout_resource_size;
  LAYOUT* layout = (LAYOUT*)malloc(sizeof(LAYOUT));
  LAYOUT_ITEM_RC* layout_table;
  struct LAYOUT_CHILD_PARAM l_param;
  POINT p;
  RECT dialog_rect;

  resource_handle = FindResource(resource, layout_name, RT_RCDATA);
  layout_table_handle = LoadResource(resource, resource_handle);
  layout_resource_size = SizeofResource(resource, resource_handle);
  layout_table = (LAYOUT_ITEM_RC*)LockResource(layout_table_handle);
  
  l_param.layout = layout;
  l_param.layout_table = layout_table;
  l_param.n = layout_resource_size/sizeof(LAYOUT_ITEM_RC);
  GetClientRect(dialog, &l_param.dialog_rect);
  p.x = l_param.dialog_rect.right;
  p.y = l_param.dialog_rect.bottom;
  ClientToScreen(dialog, &p);
  l_param.dialog_rect.right = p.x;
  l_param.dialog_rect.bottom = p.y;
  p.x = 0;
  p.y = 0;
  ClientToScreen(dialog, &p);
  l_param.dialog_rect.left = p.x;
  l_param.dialog_rect.top = p.y;
  GetWindowRect(dialog, &dialog_rect);
  layout->width = dialog_rect.right - dialog_rect.left;
  layout->height = dialog_rect.bottom - dialog_rect.top;

  layout->control_layout = 0;
  
  EnumChildWindows(dialog, layout_child, (LPARAM)&l_param);

  return layout;
}

LAYOUT*
create_layout(HANDLE resource, LPCTSTR dialog_name, LPCTSTR layout_name) {
  HGLOBAL layout_table_handle;
  HGLOBAL dialog_handle;
  HRSRC resource_handle;
  LAYOUT_ITEM_RC* layout_table;
  DLGTEMPLATE* dialog;
  DLGITEMTEMPLATE* dialog_item;
  DWORD layout_resource_size;
  unsigned int i;
  int n;
  int dialog_width;
  int dialog_height;
  int controls_count;
  LAYOUT_ITEM_LIST* prev;
  LAYOUT* layout = (LAYOUT*)malloc(sizeof(LAYOUT));
  WNDCLASS wc;
  HINSTANCE instance;
  
  instance = GetModuleHandle(0);

  memset(&wc,0,sizeof(wc));
  wc.lpfnWndProc = dialog_procedure;
  wc.cbWndExtra = DLGWINDOWEXTRA;
  wc.hInstance = instance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wc.lpszClassName = DIALOG_WITH_LAYOUT_CLASS;
  UnregisterClass(wc.lpszClassName, instance);
  RegisterClass(&wc);

  layout->control_layout = 0;
  prev  = layout->control_layout;
    
  resource_handle = FindResource(resource, layout_name, RT_RCDATA);
  layout_table_handle = LoadResource(resource, resource_handle);
  layout_resource_size = SizeofResource(resource, resource_handle);
  layout_table = (LAYOUT_ITEM_RC*)LockResource(layout_table_handle);

  resource_handle = FindResource(resource, dialog_name, RT_DIALOG);
  dialog_handle = LoadResource(resource, resource_handle);
  dialog = (DLGTEMPLATE*)LockResource(dialog_handle);
  
  dialog_item = first_item(dialog);
  
  if(is_extended_dialog(dialog)) {
    layout->width = ((DLGTEMPLATEEX*)dialog)->cx;
    layout->height = ((DLGTEMPLATEEX*)dialog)->cy;
    dialog_width = ((DLGTEMPLATEEX*)dialog)->cx;
    dialog_height = ((DLGTEMPLATEEX*)dialog)->cy;

    controls_count = ((DLGTEMPLATEEX*)dialog)->cdit;
  }
  else {
    layout->width = dialog->cx;
    layout->height = dialog->cy;
    dialog_width = dialog->cx;
    dialog_height = dialog->cy;
  
    controls_count = dialog->cdit;
  }
  
  for(n = 0; n < controls_count; n++) {
    DWORD item_id;
    int x;
    int y;
    int cx;
    int cy;
    
    if(is_extended_dialog(dialog)) {
      item_id = ((DLGITEMTEMPLATEEX*)dialog_item)->id;
      x = ((DLGITEMTEMPLATEEX*)dialog_item)->x;
      y = ((DLGITEMTEMPLATEEX*)dialog_item)->y;
      cx = ((DLGITEMTEMPLATEEX*)dialog_item)->cx;
      cy = ((DLGITEMTEMPLATEEX*)dialog_item)->cy;
    }
    else {
      item_id = dialog_item->id;
      x = dialog_item->x;
      y = dialog_item->y;
      cx = dialog_item->cx;
      cy = dialog_item->cy;
    }
    
    for(i = 0; i < layout_resource_size/sizeof(LAYOUT_ITEM_RC); i++) {
      if(layout_table[i].id == item_id) {
        LAYOUT_ITEM_LIST* item = (LAYOUT_ITEM_LIST*)malloc(sizeof(LAYOUT_ITEM_LIST));
        
        item->item.id = layout_table[i].id;
        item->item.top_left.anchor = layout_table[i].top_left_anchor;
        item->item.bottom_right.anchor = layout_table[i].bottom_right_anchor;
        item->next = 0;

        if(item->item.top_left.anchor & ANCOR_RIGHT) {
          item->item.top_left.x = x - dialog_width;
        }
    
        if(item->item.top_left.anchor & ANCOR_LEFT) {
          item->item.top_left.x = x;
        }
    
        if(item->item.top_left.anchor & ANCOR_TOP) {
          item->item.top_left.y = y;
        }
    
        if(item->item.top_left.anchor & ANCOR_BOTTOM) {
          item->item.top_left.y = y - dialog_height;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_RIGHT) {
          item->item.bottom_right.x = x - dialog_width + cx;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_LEFT) {
          item->item.bottom_right.x = x + cx;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_TOP) {
          item->item.bottom_right.y = y + cy;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_BOTTOM) {
          item->item.bottom_right.y = y - dialog_height + cy;
        }
    
        if(layout->control_layout == 0) {
          layout->control_layout = item;
          prev = (LAYOUT_ITEM_LIST*)layout->control_layout;
        } else {
          prev->next = item;
          prev = item;
        }
      }
    }

    if(is_extended_dialog(dialog)) {
      dialog_item = (DLGITEMTEMPLATE*)next_extended_item((DLGITEMTEMPLATEEX*)dialog_item);
    }
    else {
      dialog_item = next_item(dialog_item);
    }
  }
  
  FreeResource(dialog);
  FreeResource(layout_table_handle);
  
  return layout;
}

void
layout(HWND dialog) {
  LAYOUT* layout = (LAYOUT*)GetWindowLongPtr(dialog, DWLP_USER);
  
  if(layout != 0) {
    dump_layout(layout);
  }
  
  if(layout != 0) {
    LAYOUT_ITEM_LIST* current =  (LAYOUT_ITEM_LIST*)(layout->control_layout);
  
    RECT dialog_rect;
    HWND current_control;
    
    HDWP position_handle = BeginDeferWindowPos(16); /* store number of controls inside LAYOUT structure */
    
    GetClientRect(dialog, &dialog_rect);
  
    while(current != 0) {
      RECT rect;
      LAYOUT_ITEM* item = &(current->item);
      int x;
      int y;
      int w;
      int h;
      
      current_control = GetDlgItem(dialog, item->id);
  
      rect.left = item->top_left.x;
      rect.top = item->top_left.y;
      rect.right = item->bottom_right.x;
      rect.bottom = item->bottom_right.y;
      
/*      MapDialogRect(dialog, &rect);*/
  
      if(item->top_left.anchor & ANCOR_RIGHT) {
        rect.left += dialog_rect.right;
      }
  
      if(item->top_left.anchor & ANCOR_LEFT) {
        rect.left += dialog_rect.left;
      }
  
      if(item->top_left.anchor & ANCOR_TOP) {
        rect.top += dialog_rect.top;
      }
  
      if(item->top_left.anchor & ANCOR_BOTTOM) {
        rect.top += dialog_rect.bottom;
      }
  
      if(item->bottom_right.anchor & ANCOR_RIGHT) {
        rect.right += dialog_rect.right;
      }
  
      if(item->bottom_right.anchor & ANCOR_LEFT) {
        rect.right += dialog_rect.left;
      }
  
      if(item->bottom_right.anchor & ANCOR_TOP) {
        rect.bottom += dialog_rect.top;
      }
  
      if(item->bottom_right.anchor & ANCOR_BOTTOM) {
        rect.bottom += dialog_rect.bottom;
      }
  
      w = rect.right-rect.left;
      h = rect.bottom-rect.top;
      x = rect.left;
      y = rect.top;
  
      position_handle = DeferWindowPos(position_handle, current_control, 0, x, y, w, h, SWP_NOZORDER);
      
      current = current->next;
    }
    
    EndDeferWindowPos(position_handle);
  }
}
