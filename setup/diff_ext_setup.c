#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include "diff_ext_setup_res.h"

/*
const DWORD ANCOR_LEFT  =0x000001;
const DWORD ANCOR_TOP   =0x000002;
const DWORD ANCOR_RIGHT =0x000004;
const DWORD ANCOR_BOTTOM=0x000008;
*/

typedef struct {
  DWORD id;
  DWORD top_left_anchor;
  DWORD bottom_right_anchor;
} LAYOUT_ITEM_RC;

typedef struct {
  int x;
  int y;
  DWORD anchor;
} LAYOUT_COORD;

typedef struct {
  DWORD id;
  LAYOUT_COORD top_left;
  LAYOUT_COORD bottom_right;
} LAYOUT_ITEM;

typedef struct LAYOUT_ITEM_LIST {
  LAYOUT_ITEM item;
  struct LAYOUT_ITEM_LIST* next;
} LAYOUT_ITEM_LIST;

LAYOUT_ITEM_LIST* dialog_layout_root;
HANDLE resource;

static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

static BOOL
is_extended_dialog(DLGTEMPLATE* tpl) {
  return (HIWORD(tpl->style) == 0xffff);
}

static DLGITEMTEMPLATE*
first_item(DLGTEMPLATE* tpl) {
  WORD* current;
  
  current = (WORD*)(tpl+1);
  
/* skip menu */  
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;
  
/* skip class */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;

/* skip title */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;
  
  if (tpl->style & DS_SETFONT) {
    current++;
    while((*current) != 0) {
      current++;
    }
    current++;
  }
  
  return (DLGITEMTEMPLATE*)(((DWORD)current+3) & ~(DWORD)3 );
}

static DLGITEMTEMPLATE*
next_item(DLGITEMTEMPLATE* tpl) {
  WORD* current;
  WORD extra_size;
  
  tpl++;
  current = (WORD*)tpl;
  
/* skip class */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;

/* skip text */    
  if((*current) == 0xffff) {
    current++;
  }
  else {
    while((*current) != 0) {
      current++;
    }
  }
  current++;
  
/* testme!!! */  
  extra_size = *current;
  if(extra_size > 0) {
    current = (WORD*)((BYTE*)current + extra_size);
  }
  else {
    current++;
  }
  
  return (DLGITEMTEMPLATE*)(((DWORD)current+3) & ~(DWORD)3 );
}

static void
init_layout() {
  HGLOBAL layout_table_handle;
  HGLOBAL dialog_handle;
  HRSRC resource_handle;
  LAYOUT_ITEM_RC* layout_table;
  DLGTEMPLATE* dialog;
  DLGITEMTEMPLATE* dialog_item;
  DWORD layout_resource_size;
  int i;
  int n;
  LAYOUT_ITEM_LIST* prev;
  
  dialog_layout_root = (LAYOUT_ITEM_LIST*)malloc(sizeof(LAYOUT_ITEM_LIST));
  prev  = dialog_layout_root;
    
  resource_handle = FindResource(resource, MAKEINTRESOURCE(ID_MAINDIALOG_LAYOUT), RT_RCDATA);
  layout_table_handle = LoadResource(resource, resource_handle);
  layout_resource_size = SizeofResource(resource, resource_handle);
  layout_table = (LAYOUT_ITEM_RC*)LockResource(layout_table_handle);

  resource_handle = FindResource(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), RT_DIALOG);
  dialog_handle = LoadResource(resource, resource_handle);
  dialog = (DLGTEMPLATE*)LockResource(dialog_handle);
  
  dialog_item = first_item(dialog);
  
  for(n = 0; n < dialog->cdit; n++) {
    for(i = 0; i < layout_resource_size/sizeof(LAYOUT_ITEM_RC); i++) {
      if(layout_table[i].id == dialog_item->id) {
        LAYOUT_ITEM_LIST* item = (LAYOUT_ITEM_LIST*)malloc(sizeof(LAYOUT_ITEM_LIST));
        
        item->item.id = layout_table[i].id;
        item->item.top_left.anchor = layout_table[i].top_left_anchor;
        item->item.bottom_right.anchor = layout_table[i].bottom_right_anchor;
        
        if(item->item.top_left.anchor & ANCOR_RIGHT) {
          item->item.top_left.x = dialog_item->x - dialog->cx;
        }
    
        if(item->item.top_left.anchor & ANCOR_LEFT) {
          item->item.top_left.x = dialog_item->x;
        }
    
        if(item->item.top_left.anchor & ANCOR_TOP) {
          item->item.top_left.y = dialog_item->y;
        }
    
        if(item->item.top_left.anchor & ANCOR_BOTTOM) {
          item->item.top_left.y = dialog_item->y - dialog->cy;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_RIGHT) {
          item->item.bottom_right.x = dialog_item->x - dialog->cx + dialog_item->cx;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_LEFT) {
          item->item.bottom_right.x = dialog_item->x + dialog_item->cx;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_TOP) {
          item->item.bottom_right.y = dialog_item->y + dialog_item->cy;
        }
    
        if(item->item.bottom_right.anchor & ANCOR_BOTTOM) {
          item->item.bottom_right.y = dialog_item->y - dialog->cy + dialog_item->cy;
        }
    
        prev->next = item;
        prev = item;
        prev->next = 0;
      }
    }

    dialog_item = next_item(dialog_item);
  }
  
  FreeResource(dialog);
  FreeResource(layout_table_handle);
}

static void
layout(HWND hwndDlg) {
  LAYOUT_ITEM_LIST* current = dialog_layout_root->next;
  RECT dialog_rect;
  HWND current_control;
  
  GetClientRect(hwndDlg, &dialog_rect);
  
  while(current != 0) {
    RECT rect;
    LAYOUT_ITEM* item = &(current->item);
    int x;
    int y;
    int w;
    int h;
    
    current_control = GetDlgItem(hwndDlg, item->id);

    rect.left = item->top_left.x;
    rect.top = item->top_left.y;
    rect.right = item->bottom_right.x;
    rect.bottom = item->bottom_right.y;
    
    MapDialogRect(hwndDlg, &rect);

    if(item->top_left.anchor & ANCOR_RIGHT) {
      rect.left += dialog_rect.right;
    }

    if(item->top_left.anchor & ANCOR_LEFT) {
      rect.left += dialog_rect.left;
    }

    if(item->top_left.anchor & ANCOR_TOP) {
      rect.top += dialog_rect.top;
    }

    if(item->top_left.anchor & ANCOR_BOTTOM) {
      rect.top += dialog_rect.bottom;
    }

    if(item->bottom_right.anchor & ANCOR_RIGHT) {
      rect.right += dialog_rect.right;
    }

    if(item->bottom_right.anchor & ANCOR_LEFT) {
      rect.right += dialog_rect.left;
    }

    if(item->bottom_right.anchor & ANCOR_TOP) {
      rect.bottom += dialog_rect.top;
    }

    if(item->bottom_right.anchor & ANCOR_BOTTOM) {
      rect.bottom += dialog_rect.bottom;
    }

    w = rect.right-rect.left;
    h = rect.bottom-rect.top;
    x = rect.left;
    y = rect.top;

    MoveWindow(current_control, x, y, w, h, FALSE);
    
    current = current->next;
  }
}

int APIENTRY
WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc;
  HRESULT ret;
  HKEY key;
  DWORD language = 0;
  DWORD hlen;
  char language_lib[MAX_PATH];

  memset(&wc,0,sizeof(wc));
  wc.lpfnWndProc = DefDlgProc;
  wc.cbWndExtra = DLGWINDOWEXTRA;
  wc.hInstance = hinst;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wc.lpszClassName = "diff-ext-setup";
  RegisterClass(&wc);

  SetThreadLocale(MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), SORT_DEFAULT));
  InitCommonControls();

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Z\\diff_ext\\", 0, KEY_READ, &key) == ERROR_SUCCESS) {
    hlen = sizeof(DWORD);
    RegQueryValueEx(key, "language", 0, NULL, (BYTE*)&language, &hlen);

    RegCloseKey(key);

    sprintf(language_lib, "diff_ext_setup%d.dll", language);
  }

  resource = LoadLibrary(language_lib);

  if(resource == NULL)
    resource = hinst;

  ret = DialogBox(resource, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC) DialogFunc);
/*
  {
    char* message;

    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      0,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &message,
      0,
      0
    );

    MessageBox(0, message, "GetLastError", MB_OK | MB_ICONINFORMATION);

    LocalFree(message);
  }
*/
  FreeLibrary(resource);

  return ret;
}

static void
InitializeApp(HWND hDlg,WPARAM wParam, LPARAM lParam) {
  HKEY key;
  char command[MAX_PATH];
  char home[MAX_PATH];
  DWORD hlen;

  ZeroMemory(command, MAX_PATH);

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Z\\diff_ext\\", 0, KEY_READ, &key) == ERROR_SUCCESS) {
    HANDLE file;
    WIN32_FIND_DATA file_info;
    DWORD language;
    char prefix[] = "diff_ext_setup";
    char root[] = "????";
    char suffix[] = ".dll";
    TCHAR* locale_info;
    int locale_info_size;
    int curr = 0;

    hlen = MAX_PATH;
    RegQueryValueEx(key, "diff", 0, NULL, (BYTE*)command, &hlen);

    hlen = sizeof(DWORD);
    RegQueryValueEx(key, "language", 0, NULL, (BYTE*)&language, &hlen);

    hlen = MAX_PATH;
    RegQueryValueEx(key, "home", 0, NULL, (BYTE*)home, &hlen);

    locale_info_size = GetLocaleInfo(1033, LOCALE_SNATIVELANGNAME, 0, 0);
    locale_info = (TCHAR*)malloc(locale_info_size);
    GetLocaleInfo(1033, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

    SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
    SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_SETITEMDATA, curr, 1033);

    if(language == 1033)
      SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_SETCURSEL, curr, 0);

    free(locale_info);
    curr++;

    strcat(home, "\\");
    strcat(home, prefix);
    strcat(home, root);
    strcat(home, suffix);

    file = FindFirstFile(home, &file_info);
    if(file != INVALID_HANDLE_VALUE) {
      BOOL stop = FALSE;

      while(stop == FALSE) {
        char* str = file_info.cFileName+sizeof(prefix)/sizeof(char)-1;
        DWORD lang_id;

        str[sizeof(root)/sizeof(char)-1] = 0;

        lang_id = atoi(str);

	locale_info_size = GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, 0, 0);
	locale_info = (TCHAR*)malloc(locale_info_size);
	GetLocaleInfo(lang_id, LOCALE_SNATIVELANGNAME, locale_info, locale_info_size);

	SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)locale_info);
	SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_SETITEMDATA, curr, lang_id);

        if(lang_id == language)
          SendDlgItemMessage(hDlg, ID_LANGUAGE, CB_SETCURSEL, curr, 0);

	free(locale_info);

	stop = !FindNextFile(file, &file_info);
	curr++;
      }

      FindClose(file);
    }

    RegCloseKey(key);
  }

  SetDlgItemText(hDlg, ID_DIFF_COMMAND, command);
  SendDlgItemMessage(hDlg, ID_DIFF_COMMAND, EM_SETLIMITTEXT, MAX_PATH, 0);
  
  init_layout();
}

static BOOL CALLBACK
DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
  BOOL ret = FALSE;

  switch (msg) {
    case WM_INITDIALOG:
      InitializeApp(hwndDlg,wParam,lParam);
      ret = TRUE;
      break;

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case ID_DIFF_COMMAND: {
            static int first = 1;
            if((first != 0) && (HIWORD(wParam) == EN_SETFOCUS)) {
              SendDlgItemMessage(hwndDlg, ID_DIFF_COMMAND, EM_SETSEL, 0, 0);
              SendDlgItemMessage(hwndDlg, ID_DIFF_COMMAND, EM_SETMARGINS, EC_RIGHTMARGIN, MAKELPARAM(0, 30));
              first = 0;
            }
          }
          break;

        case IDOK: {
            HKEY key;
            char command[MAX_PATH];
	    LRESULT language;
	    LRESULT idx;

            GetDlgItemText(hwndDlg, ID_DIFF_COMMAND, command, MAX_PATH);
	    idx = SendDlgItemMessage(hwndDlg, ID_LANGUAGE, CB_GETCURSEL, 0, 0);
	    language = SendDlgItemMessage(hwndDlg, ID_LANGUAGE, CB_GETITEMDATA, idx, 0);

            RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Z\\diff_ext\\", 0, KEY_SET_VALUE, &key);
            RegSetValueEx(key, "diff", 0, REG_SZ, (const BYTE*)command, strlen(command));
            RegSetValueEx(key, "language", 0, REG_DWORD, (const BYTE*)&language, sizeof(language));
            RegCloseKey(key);

            EndDialog(hwndDlg,1);
            ret = TRUE;
          }
          break;

        case IDCANCEL:
          EndDialog(hwndDlg,0);
          ret = TRUE;
          break;

        case ID_BROWSE: {
            OPENFILENAME ofn;
            char szFile[MAX_PATH] = "";
            DWORD err;

            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = hwndDlg;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "Applications (*.exe)\0*.EXE\0All (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

            if(GetOpenFileName(&ofn) == TRUE)
              SetDlgItemText(hwndDlg, ID_DIFF_COMMAND, ofn.lpstrFile);

            err = CommDlgExtendedError();

            ret = TRUE;
          }
          break;
      }
      break;

    case WM_GETMINMAXINFO: {
        MINMAXINFO* min_max_info = (MINMAXINFO*)lParam;
        RECT client;
        WINDOWINFO info;

        client.top = 0;
        client.bottom = 95;
        client.left = 0;
        client.right = 195;

        MapDialogRect(hwndDlg, &client);
        GetWindowInfo(hwndDlg, &info);
        AdjustWindowRectEx(&client, info.dwStyle, FALSE, info.dwExStyle);

        min_max_info->ptMinTrackSize.x = client.right-client.left;
        min_max_info->ptMinTrackSize.y = client.bottom-client.top;
        ret = TRUE;
      }
      break;

    case WM_SIZE: {
/**/
        layout(hwndDlg);
/* redraw them all*/
        InvalidateRect(hwndDlg, 0, TRUE);
        UpdateWindow(hwndDlg);

        ret = TRUE;
      }
      break;

    case WM_CLOSE:
      EndDialog(hwndDlg,0);
      ret = TRUE;
      break;
  }

  return ret;
}
