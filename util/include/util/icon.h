/*
 * Copyright (c) 2006 Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __icon_h__
#define __icon_h__

#include <windows.h>

class ICON_REF_COUNTER;

class ICON {
  public:
    ICON(const HICON& icon);
    ICON(const ICON& icon);
    ~ICON();
  
    operator HICON() const;
  
    ICON& operator=(const ICON& icon);
    ICON& operator=(const HICON& icon);

  private:
    void acquire(ICON_REF_COUNTER*);
    void release();
  
  private:
    ICON_REF_COUNTER* _counter;
};

#endif // __icon_h__
