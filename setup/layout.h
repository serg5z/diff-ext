#ifndef __layout_h__
#define __layout_h__

#include <windows.h>

typedef struct {
  int width;
  int height;
  void* control_layout;
} LAYOUT;

LAYOUT* create_layout(HANDLE resource, LPCTSTR dialog_name, LPCTSTR layout_name);
void layout(HWND hwndDlg);

#endif /* __layout_h__ */
