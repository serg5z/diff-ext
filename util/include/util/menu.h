/*
 * Copyright (c) 2006 Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __menu_h__
#define __menu_h__

#include <windows.h>

#ifdef __cplusplus
#include <util/string.h>
#include <util/icon.h>

class MENUITEM {
  public:
    MENUITEM();
    MENUITEM(UINT id, STRING text, HICON icon=0);  
    MENUITEM(UINT id, STRING text, ICON icon);  
    MENUITEM(const MENUITEM& item);
  
    virtual ~MENUITEM();
  
    virtual void measure(MEASUREITEMSTRUCT* mis);
    virtual void draw(DRAWITEMSTRUCT* dis);
  
    MENUITEM& operator=(const MENUITEM& item);
  
    virtual MENUITEMINFO* item_info();
  
  private:
    STRING _text;
    ICON _icon;
    UINT _id;
};

class SUBMENU : public MENUITEM {
  public:
    SUBMENU();
    SUBMENU(HMENU menu, UINT id=0, STRING text=TEXT(""), HICON icon=0);
    SUBMENU(HMENU menu, UINT id, STRING text, ICON icon);
    SUBMENU(const SUBMENU& menu);
  
    virtual ~SUBMENU();
  
    virtual void insert(MENUITEM& item, UINT position);
    virtual void append(MENUITEM& item, UINT id = 0);
  
    SUBMENU& operator=(const SUBMENU& item);
  
  protected:
    virtual MENUITEMINFO* item_info();
  
  private:
    HMENU _menu;
    bool _own_menu;
};

extern "C" {
#else /*__cplusplus*/
typedef void* MENUITEM;

MENUITEM create_menu_item(UINT id, LPTSTR text, HICON icon);
MENUITEM create_submenu(HMENU menu, UINT id, LPTSTR text, HICON icon);
void delete_menu_item(MENUITEM item);
  
void insert(MENUITEM menu, MENUITEM item/*MENUITEM or SUBMENU handle*/, UINT position);
void append(MENUITEM menu, MENUITEM item/*MENUITEM or SUBMENU handle*/, UINT id);
void measure(MENUITEM menuitem, MEASUREITEMSTRUCT* mis);
void draw(MENUITEM menuitem, DRAWITEMSTRUCT* dis);
#endif /*__cplusplus*/
#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* __menu_h__ */
