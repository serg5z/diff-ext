/*
 * Copyright (c) 2006, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms of the
 * BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __dialog_h__
#define __dialog_h__

#include <windows.h>

BOOL set_url(HWND hwndCtl, LPCTSTR strURL);
BOOL set_url3(HWND hwndParent, UINT uiCtlId, LPCTSTR strURL);
#endif /* __dialog_h__ */
