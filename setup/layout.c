#include "layout.h"

static const DWORD ANCOR_LEFT  =0x000001;
static const DWORD ANCOR_TOP   =0x000002;
static const DWORD ANCOR_RIGHT =0x000004;
static const DWORD ANCOR_BOTTOM=0x000008;

typedef struct {
  WORD dlgVer;
  WORD signature;
  DWORD helpID;
  DWORD exStyle;
  DWORD style;
  WORD cDlgItems;
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
  
  return (DLGITEMTEMPLATE*)(((DWORD)current+3) & ~(DWORD)3 );
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
  int i;
  int n;
  int controls_count;
  LAYOUT_ITEM_LIST* prev;
  LAYOUT* layout = (LAYOUT*)malloc(sizeof(LAYOUT));

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
    controls_count = ((DLGTEMPLATEEX*)dialog)->cDlgItems;
  }
  else {
    controls_count = dialog->cdit;
  }
  
  layout->width = dialog->cx;
  layout->height = dialog->cy;
  
  for(n = 0; n < controls_count; n++) {
    for(i = 0; i < layout_resource_size/sizeof(LAYOUT_ITEM_RC); i++) {
      if(layout_table[i].id == dialog_item->id) {
        LAYOUT_ITEM_LIST* item = (LAYOUT_ITEM_LIST*)malloc(sizeof(LAYOUT_ITEM_LIST));
        
        item->item.id = layout_table[i].id;
        item->item.top_left.anchor = layout_table[i].top_left_anchor;
        item->item.bottom_right.anchor = layout_table[i].bottom_right_anchor;
        item->next = 0;

        if(item->item.top_left.anchor & ANCOR_RIGHT) {
          item->item.top_left.x = dialog_item->x - dialog->cx;
        }
    
        if(item->item.top_left.anchor & ANCOR_LEFT) {
          item->item.top_left.x = dialog_item->x;
        }
    
        if(item->item.top_left.anchor & ANCOR_TOP) {
          item->item.top_left.y = dialog_item->y;
        }
    
        if(item->item.top_left.anchor & ANCOR_BOTTOM) {
          item->item.top_left.y = dialog_item->y - dialog->cy;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_RIGHT) {
          item->item.bottom_right.x = dialog_item->x - dialog->cx + dialog_item->cx;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_LEFT) {
          item->item.bottom_right.x = dialog_item->x + dialog_item->cx;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_TOP) {
          item->item.bottom_right.y = dialog_item->y + dialog_item->cy;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_BOTTOM) {
          item->item.bottom_right.y = dialog_item->y - dialog->cy + dialog_item->cy;
        }
    
        if(layout->control_layout == 0) {
          layout->control_layout = item;
          prev = (LAYOUT_ITEM_LIST*)layout->control_layout;
        }
        else {
          prev->next = item;
          prev = item;
        }
      }
    }

    dialog_item = next_item(dialog_item);
  }
  
  FreeResource(dialog);
  FreeResource(layout_table_handle);
  
  return layout;
}

void
layout(HWND hwndDlg) {
  LAYOUT* layout = (LAYOUT*)GetWindowLongPtr(hwndDlg, DWLP_USER);
  LAYOUT_ITEM_LIST* current =  (LAYOUT_ITEM_LIST*)(layout->control_layout);

  RECT dialog_rect;
  HWND current_control;
  
  GetClientRect(hwndDlg, &dialog_rect);
  
  while(current != 0) {
    RECT rect;
    LAYOUT_ITEM* item = &(current->item);
    int x;
    int y;
    int w;
    int h;
    
    current_control = GetDlgItem(hwndDlg, item->id);

    rect.left = item->top_left.x;
    rect.top = item->top_left.y;
    rect.right = item->bottom_right.x;
    rect.bottom = item->bottom_right.y;
    
    MapDialogRect(hwndDlg, &rect);

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

    MoveWindow(current_control, x, y, w, h, FALSE);
    
    current = current->next;
  }
}
