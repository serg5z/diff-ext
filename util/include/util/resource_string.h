/*
 * Copyright (c) 2006, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __resource_string_h__
#define __resource_string_h__

#include <windows.h>

#ifdef __cplusplus
extern "C" 
#endif
void load_resource_string(HINSTANCE  resource, UINT string_id, TCHAR* string, int length, TCHAR* default_value);

#endif /* __resource_string_h__ */
