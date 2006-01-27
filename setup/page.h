/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#ifndef __page_h__
#define __page_h__

#include <windows.h>

typedef struct __PAGE {
  HWND page;
  void (*apply)(struct __PAGE*);
} PAGE;

#endif /*__page_h__*/
