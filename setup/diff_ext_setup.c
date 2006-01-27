/*
 * Copyright (c) 2003-2006, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include <tchar.h>

#include "layout.h"
#include "page.h"

#include "diff_ext_setup.rh"

typedef struct {
  int x;
  int y;
  int width;
  int height;
} WINDOW_PLACEMENT;

static BOOL CALLBACK main_dialog_func(HWND dialog, UINT msg, WPARAM wParam, LPARAM lParam);
extern PAGE* create_options_page(HANDLE resource, HWND parent);
extern PAGE* create_debug_page(HANDLE resource, HWND parent);
extern void about(HANDLE resource, HWND parent);

static HANDLE resource;
static WINDOW_PLACEMENT* window_placement = 0;
static PAGE* pages[2];

static WNDPROC old_button_procedure;
static LRESULT
new_button_procedure(HWND button, UINT message, WPARAM w_param, LPARAM l_param) {
  LRESULT result= CallWindowProc(old_button_procedure, button, message, w_param, l_param);
  if(message == WM_LBUTTONUP) {
    MessageBox(0, TEXT("A button has been clicked"), TEXT("Info"), MB_OK);
  }
  
  return result;
}

static void
subclass_button() {
  HWND tmp;
  
  tmp = CreateWindow(TEXT("BUTTON"), TEXT("tmp"), WS_POPUP, 0, 0, 50, 50, 0, NULL, GetModuleHandle(0), NULL);
  
  old_button_procedure = (WNDPROC)SetClassLong(tmp, GCL_WNDPROC, (DWORD)new_button_procedure);
  
  DestroyWindow(tmp);
}

#ifdef __MINGW32__
/*
Wait for wide startup module from MinGW
*/
int APIENTRY
WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show) {
#else  
int APIENTRY
_tWinMain(HINSTANCE instance, HINSTANCE previous, LPTSTR command_line, int show) {
#endif  
  HRESULT exit = ID_APPLY;
  HKEY key;
  DWORD language = 0;
  DWORD hlen;
  TCHAR language_lib[MAX_PATH];
  HGLOBAL dialog_handle;
  HRSRC resource_handle;
  DLGTEMPLATE* dialog;
  
  subclass_button();

  while(exit == ID_APPLY) {
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
      hlen = sizeof(DWORD);
      RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&language, &hlen);
  
      RegCloseKey(key);
  
      _stprintf(language_lib, TEXT("diff_ext_setup%ld.dll"), language);
    }
  
    resource = LoadLibrary(language_lib);
  
    if(resource == NULL) {
      resource = instance;
    }
  
    SetThreadLocale(LOCALE_USER_DEFAULT);
    InitCommonControls();
  
    resource_handle = FindResource(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), RT_DIALOG);
    dialog_handle = LoadResource(resource, resource_handle);
    dialog = (DLGTEMPLATE*)LockResource(dialog_handle);
  
  /*  can not do because have to specify hinst as module instance and load dialog from translated resource only dll...
    ret = DialogBox(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)main_dialog_func);
  */  
    exit = DialogBoxIndirect(instance, dialog, NULL, (DLGPROC)main_dialog_func);

    FreeResource(dialog);
    FreeLibrary(resource);
  }
  
  return exit;
}

static void
init(HWND dialog, WPARAM not_used, LPARAM l_param) {
  HWND tab = GetDlgItem(dialog, ID_TAB);
  RECT rect;
  TCITEM item1;
  TCITEM item2;
  HMODULE uxtheme_library = LoadLibrary(TEXT("uxtheme.dll"));
  TCHAR page_1_label[32];
  TCHAR page_2_label[32];
  int resource_string_length;

  SetClassLong(dialog, GCL_HICON, (long)LoadIcon(resource, MAKEINTRESOURCE(MAIN_ICON)));
  pages[0] = create_options_page(resource, dialog);
  pages[1] = create_debug_page(resource, dialog);

  if(uxtheme_library != 0) {
    FARPROC EnableThemeDialogTexture = GetProcAddress(uxtheme_library, "EnableThemeDialogTexture");
    
    if(EnableThemeDialogTexture != 0) {
      unsigned int i;
      const int ETDT_DISABLE = 1;
      const int ETDT_ENABLE = 2;
      const int ETDT_USETABTEXTURE = 4;
      const int ETDT_ENABLETAB = ETDT_ENABLE | ETDT_USETABTEXTURE;
      for(i = 0; i < sizeof(pages)/sizeof(pages[0]); i++) {
	(EnableThemeDialogTexture)(pages[i]->page, ETDT_ENABLETAB);
      }
    }
  }

  ZeroMemory(&item1, sizeof(TCITEM));
  ZeroMemory(&item2, sizeof(TCITEM));

  resource_string_length = LoadString(resource, OPTIONS_STR, page_1_label, sizeof(page_1_label)/sizeof(page_1_label[0]));
  
  if(resource_string_length == 0) {
    HMODULE instance = GetModuleHandle(0);
    resource_string_length = LoadString(instance, OPTIONS_STR, page_1_label, sizeof(page_1_label)/sizeof(page_1_label[0]));
    
    if(resource_string_length == 0) {
      lstrcpy(page_1_label, TEXT("Options"));
      MessageBox(0, TEXT("Can not load 'OPTIONS_STR' string resource"), TEXT("ERROR"), MB_OK);
    }
  }
  
  item1.mask = TCIF_TEXT | TCIF_PARAM;
  item1.pszText = page_1_label;
  item1.lParam = (LPARAM)0;

  resource_string_length = LoadString(resource, LOGGING_STR, page_2_label, sizeof(page_2_label)/sizeof(page_2_label[0]));
  
  if(resource_string_length == 0) {
    HMODULE instance = GetModuleHandle(0);
    resource_string_length = LoadString(instance, LOGGING_STR, page_2_label, sizeof(page_2_label)/sizeof(page_2_label[0]));
    
    if(resource_string_length == 0) {
      lstrcpy(page_2_label, TEXT("Logging"));
      MessageBox(0, TEXT("Can not load 'LOGGING_STR' string resource"), TEXT("ERROR"), MB_OK);
    }
  }
  
  item2.mask = TCIF_TEXT | TCIF_PARAM;
  item2.pszText = page_2_label;
  item2.lParam = (LPARAM)0;

  TabCtrl_InsertItem(tab, 1, &item1);
  TabCtrl_InsertItem(tab, 2, &item2);  
  
  attach_layout(resource, dialog, MAKEINTRESOURCE(ID_MAINDIALOG_LAYOUT));
  
  if(window_placement == 0) {
    RECT rect;
    window_placement = (WINDOW_PLACEMENT*)malloc(sizeof(WINDOW_PLACEMENT));
    
    GetWindowRect(dialog, &rect);
    window_placement->x = rect.left;
    window_placement->y = rect.top;
    window_placement->width = rect.right-rect.left;
    window_placement->height = rect.bottom-rect.top;
  } else {
    MoveWindow(dialog, window_placement->x, window_placement->y, window_placement->width, window_placement->height, TRUE);
  }
  
  GetClientRect(tab, &rect);
  TabCtrl_AdjustRect(tab, FALSE, &rect);
  MapWindowPoints(tab, dialog, (LPPOINT)&rect, 2);  
  SetWindowPos(pages[0]->page, HWND_TOP, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW);
  layout(pages[0]->page);
}

static BOOL CALLBACK
main_dialog_func(HWND dialog, UINT msg, WPARAM wParam, LPARAM lParam) {
  BOOL ret = FALSE;

  switch(msg) {
    case WM_INITDIALOG:
      init(dialog,wParam,lParam);
      ret = TRUE;
      break;

    case WM_COMMAND:
      switch(LOWORD(wParam)) {
	case ID_ABOUT:
	  about(resource, dialog);
	  break;
	
        case ID_APPLY:
        case IDOK: {
            HKEY key;
	    LRESULT language = 1033;
	    LRESULT old_language = 1033;
	    DWORD hlen;
	    unsigned int i;

	    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
	      hlen = sizeof(DWORD);
	      if(RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&old_language, &hlen) != ERROR_SUCCESS) {
		old_language = 1033;
	      }

	      RegCloseKey(key);
	    }
	    
	    for(i = 0; i < sizeof(pages)/sizeof(pages[0]); i++) {
	      pages[i]->apply(pages[i]);
	    }
	    
	    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
	      hlen = sizeof(DWORD);
	      if(RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&language, &hlen) != ERROR_SUCCESS) {
		language = 1033;
	      }

	      RegCloseKey(key);
	    }
	    
	    if((LOWORD(wParam) == IDOK) || (language != old_language)) {
	      EndDialog(dialog, LOWORD(wParam));
	    }
	    
            ret = TRUE;
          }
          break;

        case IDCANCEL:
          EndDialog(dialog, IDCANCEL);
          ret = TRUE;
          break;
      }
      break;

    case WM_MOVE: {
        RECT rect;
        
        GetWindowRect(dialog, &rect);
        window_placement->x = rect.left;
        window_placement->y = rect.top;
        window_placement->width = rect.right-rect.left;
        window_placement->height = rect.bottom-rect.top;
      }
      break;
      
    case WM_SIZE: {
        RECT rect;
        RECT tab_rect;
        HWND tab = GetDlgItem(dialog, ID_TAB);
        int page = TabCtrl_GetCurSel(tab);
      
	GetClientRect(tab, &tab_rect);

	TabCtrl_AdjustRect(tab, FALSE, &tab_rect);

	MapWindowPoints(tab, dialog, (LPPOINT)&tab_rect, 2);
      
        MoveWindow(pages[page]->page, tab_rect.left, tab_rect.top, tab_rect.right-tab_rect.left, tab_rect.bottom-tab_rect.top, TRUE);
        layout(pages[page]->page);
      
        GetWindowRect(dialog, &rect);
        window_placement->x = rect.left;
        window_placement->y = rect.top;
        window_placement->width = rect.right-rect.left;
        window_placement->height = rect.bottom-rect.top;

        ret = TRUE;
      }
      break;
      
    case WM_NOTIFY: {
	LPNMHDR data = (LPNMHDR)lParam;

	if(data->code == TCN_SELCHANGE) {
	  if(data->idFrom == ID_TAB) {
	    int page = TabCtrl_GetCurSel(data->hwndFrom);
	    unsigned int i;
	    RECT rect;

	    GetClientRect(data->hwndFrom, &rect);
	    TabCtrl_AdjustRect(data->hwndFrom, FALSE, &rect);
	    MapWindowPoints(data->hwndFrom, dialog, (LPPOINT)&rect, 2);
	    
	    for(i = 0; i < sizeof(pages)/sizeof(pages[0]); i++) {
	      if(i == page) {
		SetWindowPos(pages[i]->page, HWND_TOP, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW);
		layout(pages[i]->page);
	      } else {
	        ShowWindow(pages[i]->page, FALSE);
	      }
	    }
	  }
	}
        
        ret = TRUE;
      }
      break;

    case WM_CLOSE:
      EndDialog(dialog,0);
      ret = TRUE;
      break;
  }

  return ret;
}
