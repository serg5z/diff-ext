/*
 * Copyright (c) 2006, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms of the
 * BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __menu_h__
#define __menu_h__

#include <windows.h>

#ifdef __cplusplus
#include <util/string.h>

class MENUITEM {
  public:
    MENUITEM();
    MENUITEM(STRING text, HICON icon=0);  
    MENUITEM(const MENUITEM& item);
  
    virtual ~MENUITEM();
  
    virtual void insert(HMENU menu, UINT id, UINT position, BOOL by_position=TRUE);
    virtual void measure(MEASUREITEMSTRUCT* mis);
    virtual void draw(DRAWITEMSTRUCT* dis);
  
    MENUITEM& operator=(const MENUITEM& item);
  
  protected:
    STRING _text;
    HICON _icon;
};

class SUBMENU : public MENUITEM {
  public:
    SUBMENU();
    SUBMENU(HMENU menu, STRING text, HICON icon=0);
    SUBMENU(const SUBMENU& menu);
  
    virtual void insert(HMENU menu, UINT id, UINT position, BOOL by_position=TRUE);
  
    SUBMENU& operator=(const SUBMENU& item);
  
  private:
    HMENU _menu;
};

extern "C" {
#else /*__cplusplus*/
typedef void* MENUITEM;

MENUITEM create_menu_item();
MENUITEM create_submenu();
  
void insert(MENUITEM menuitem/*MENUITEM or SUBMENU handle*/);
void measure(MENUITEM menuitem, MEASUREITEMSTRUCT* mis);
void draw(MENUITEM menuitem, DRAWITEMSTRUCT* dis);
#endif /*__cplusplus*/
}

#endif /* __menu_h__ */
