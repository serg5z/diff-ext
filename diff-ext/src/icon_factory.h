/*
 * Copyright (c) 2006 Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __icon_factory_h__
#define __icon_factory_h__

#include <windows.h>

class ICON_FACTORY {
  public:
    virtual ~ICON_FACTORY();
    HICON diff_icon();
    HICON diff3_icon();
    HICON diff_later_icon();
    HICON diff_with_icon();
    HICON diff3_with_icon();
    HICON clear_icon();
  
    static ICON_FACTORY* instance();
  
  private:
    ICON_FACTORY();
    ICON_FACTORY(const ICON_FACTORY&) {}
    ICON_FACTORY& operator=(const ICON_FACTORY&) { return *this; }
};

#endif // __icon_factory_h__
