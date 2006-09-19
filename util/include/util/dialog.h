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

void SetDlgItemUrl(HWND dialog, int id, LPTSTR url); 

#define UCF_TXT_DEFAULT  0 
#define UCF_TXT_LEFT     0
#define UCF_TXT_RIGHT    1
#define UCF_TXT_HCENTER  2
#define UCF_TXT_TOP      0
#define UCF_TXT_VCENTER  4
#define UCF_TXT_BOTTOM   8 
#define UCF_LNK_VISITED 16 
#define UCF_KBD         32 
#define UCF_FIT         64 
BOOL urlctrl_set(HWND,TCHAR *url,COLORREF *unvisited,COLORREF *visited,DWORD flags);

#endif /* __dialog_h__ */
