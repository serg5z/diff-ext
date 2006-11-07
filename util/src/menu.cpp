/*
 * Copyright (c) 2006, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#include <util/menu.h>

MENUITEM::MENUITEM() {
}

MENUITEM::MENUITEM(UINT id, STRING text, HICON icon) : _text(text), _icon(icon), _id(id) {
}

MENUITEM::MENUITEM(const MENUITEM& item) {
  _text = item._text;
  _icon = item._icon;
  _id = item._id;
}

MENUITEM::~MENUITEM() {
  if(_icon != 0) {
    DestroyIcon(_icon);
  }
}

MENUITEMINFO* 
MENUITEM::item_info() {
  MENUITEMINFO* result = new MENUITEMINFO;
  
  ZeroMemory(result, sizeof(MENUITEMINFO));
  result->cbSize = sizeof(MENUITEMINFO);
  
  if(_icon == 0) {
    result->fMask = MIIM_ID | MIIM_TYPE;
    result->fType = MFT_STRING;
    result->wID = _id;
    result->dwTypeData = _text;
  } else {
    result->fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA;
    result->fType = MFT_OWNERDRAW;
    result->wID = _id;
    result->dwItemData = (ULONG_PTR)this;
  }

  return  result;
}

void 
MENUITEM::measure(MEASUREITEMSTRUCT* mis) {
  NONCLIENTMETRICS ncm;

  mis->itemWidth = 0;
  mis->itemHeight = 0;
  
  ncm.cbSize = sizeof(NONCLIENTMETRICS);
  
  if(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0) != 0) {
    HDC dc = CreateCompatibleDC(0);
    
    if(dc != 0) {
      HFONT font = CreateFontIndirect(&ncm.lfMenuFont);
      
      if(font != 0) {
        SIZE size;
        HFONT old_font = (HFONT)SelectObject(dc, font);
        unsigned int icon_width = 0;
        unsigned int icon_height = 0;
        
        if(_icon != 0) {
          ICONINFO ii;
          GetIconInfo(_icon, &ii);
          HBITMAP hb = ii.hbmColor;
          BITMAP bm;
          
          if(hb == 0) {
            hb = ii.hbmMask;
          }
          
          GetObject(hb, sizeof(bm), &bm);
          
          if(hb == ii.hbmMask) {
            bm.bmHeight /= 2;
          }
          
          icon_width = bm.bmWidth;
          icon_height = bm.bmHeight;
          
          if(ii.hbmColor != 0) {
            DeleteObject(ii.hbmColor);
          }
          
          if(ii.hbmMask != 0) {
            DeleteObject(ii.hbmMask);
          }
        }
        
        GetTextExtentPoint32(dc, _text, lstrlen(_text), &size);
        LPtoDP(dc, (POINT*)&size, 1);

        SelectObject(dc, old_font);
        DeleteObject(font);
        DeleteDC(dc);

        mis->itemWidth = size.cx + 3 + icon_width; //width of string + width of icon + space between ~icon~text~
        mis->itemHeight = max(size.cy, ncm.iMenuHeight);     
        mis->itemHeight = max(mis->itemHeight, icon_height+2)+1;     
      }
    }
  }
}

void 
MENUITEM::draw(DRAWITEMSTRUCT* dis) {
  NONCLIENTMETRICS ncm;
  COLORREF text;
  COLORREF background;
  HFONT font;
  HFONT menu_font;
  
  ncm.cbSize = sizeof(NONCLIENTMETRICS);
  SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
  menu_font = CreateFontIndirect(&ncm.lfMenuFont);
  
  if (dis->itemState & ODS_SELECTED) {
    if (dis->itemState & ODS_GRAYED) {
      text = SetTextColor(dis->hDC, GetSysColor(COLOR_GRAYTEXT));
    } else {
      text = SetTextColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
    }
    
    background = SetBkColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHT));
  } else { 
    text = SetTextColor(dis->hDC, GetSysColor(COLOR_MENUTEXT)); 
    background = SetBkColor(dis->hDC, GetSysColor(COLOR_MENU)); 
  } 

  font = (HFONT)SelectObject(dis->hDC, menu_font);
  
  ExtTextOut(dis->hDC, dis->rcItem.left, dis->rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &dis->rcItem, 0, 0, 0); 
  
  if(_icon != 0) {
    ICONINFO ii;
    GetIconInfo(_icon, &ii);
    HBITMAP hb = ii.hbmColor;
    BITMAP bm;
    
    if(hb == 0) {
      hb = ii.hbmMask;
    }
    
    GetObject(hb, sizeof(bm), &bm);
    
    if(hb == ii.hbmMask) {
      bm.bmHeight /= 2;
    }
    
    DrawIconEx(dis->hDC, dis->rcItem.left+1, 1+(dis->rcItem.bottom+dis->rcItem.top-bm.bmHeight)/2, _icon, bm.bmWidth, bm.bmHeight, 0, 0, DI_NORMAL);    
    dis->rcItem.left += bm.bmWidth+3;
    
    if(ii.hbmColor != 0) {
      DeleteObject(ii.hbmColor);
    }
    
    if(ii.hbmMask != 0) {
      DeleteObject(ii.hbmMask);
    }
  } else {
    dis->rcItem.left += GetSystemMetrics(SM_CXSMICON)+3;
  }
  
  int y;
  SIZE size = {0, 0};
  
  GetTextExtentPoint32(dis->hDC, _text, lstrlen(_text), &size);
//          LPtoDP(dis->hDC, (POINT*)&size, 1);
  y = ((dis->rcItem.bottom - dis->rcItem.top) - size.cy) / 2;
  y = dis->rcItem.top + (y >= 0 ? y : 0);
  ExtTextOut(dis->hDC, dis->rcItem.left, y, ETO_OPAQUE, &dis->rcItem, _text, lstrlen(_text), 0); 
  
  SelectObject(dis->hDC, font);
  DeleteObject(menu_font);
  if (dis->itemState & ODS_SELECTED) {
    SetTextColor(dis->hDC, text);
    SetBkColor(dis->hDC, background);
  }
}

MENUITEM&
MENUITEM::operator=(const MENUITEM& item) {
  _text = item._text;
  _icon = item._icon;
  _id = item._id;
  
  return *this;
}

SUBMENU::SUBMENU() : _own_menu(true) {
  _menu = CreateMenu();
}

SUBMENU::SUBMENU(HMENU menu, UINT id, STRING text, HICON icon) : MENUITEM(id, text, icon), _menu(menu), _own_menu(false) {
  if(menu == 0) {
    _menu = CreateMenu();
    _own_menu = true;
  }
}

SUBMENU::SUBMENU(const SUBMENU& menu) : MENUITEM(menu) {
  _menu = menu._menu;
  _own_menu = menu._own_menu;
}

SUBMENU::~SUBMENU() {
  if(_own_menu) {
    DestroyMenu(_menu);
  }
}

void 
SUBMENU::insert(MENUITEM& item, UINT position) {
  MENUITEMINFO* info = item.item_info();
  
  if(info != 0) {
    InsertMenuItem(_menu, position, TRUE, info);
    
    delete info;
  }
}

void 
SUBMENU::append(MENUITEM& item, UINT id) {
  MENUITEMINFO* info = item.item_info();
  
  if(info != 0) {
    InsertMenuItem(_menu, id, FALSE, info);
    
    delete info;
  }
}

SUBMENU&
SUBMENU::operator=(const SUBMENU& menu) {
  MENUITEM::operator=(menu);
    
  _menu = menu._menu;
  _own_menu = menu._own_menu;
  
  return *this;
}

MENUITEMINFO* 
SUBMENU::item_info() {
  MENUITEMINFO* result = MENUITEM::item_info();
  
  if(result != 0) {
    result->fMask |= MIIM_SUBMENU;
    result->hSubMenu = _menu;
  }
  
  return result;
}

extern "C" {
void* 
create_menu_item(UINT id, LPTSTR text, HICON icon) {
  return new MENUITEM(id, text, icon);
}

void* 
create_submenu(HMENU menu, UINT id, LPTSTR text, HICON icon) {
  return new SUBMENU(menu, id, text, icon);
}

void
delete_menu_item(void* item) {
  delete (MENUITEM*)item;
}
  
void
insert(void* menu, void* item/*MENUITEM or SUBMENU handle*/, UINT position) {
  ((SUBMENU*)menu)->insert(*(MENUITEM*)item, position);
}

void
append(void* menu, void* item/*MENUITEM or SUBMENU handle*/, UINT id) {
  ((SUBMENU*)menu)->append(*(MENUITEM*)item, id);
}

void
measure(void* menuitem, MEASUREITEMSTRUCT* mis) {
  ((MENUITEM*)menuitem)->measure(mis);
}

void
draw(void* menuitem, DRAWITEMSTRUCT* dis) {
  ((MENUITEM*)menuitem)->draw(dis);
}
}
