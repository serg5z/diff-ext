#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include "diff_ext_setup_res.h"

static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY
WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc;

  memset(&wc,0,sizeof(wc));
  wc.lpfnWndProc = DefDlgProc;
  wc.cbWndExtra = DLGWINDOWEXTRA;
  wc.hInstance = hinst;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wc.lpszClassName = "diff-ext-setup";
  RegisterClass(&wc);

  InitCommonControls();

  return DialogBox(hinst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC) DialogFunc);
}

static void
InitializeApp(HWND hDlg,WPARAM wParam, LPARAM lParam) {
  HKEY key;
  char command[MAX_PATH];
  DWORD hlen;

  ZeroMemory(command, MAX_PATH);

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Z\\diff_ext\\", 0, KEY_READ, &key) == ERROR_SUCCESS) {
    hlen = MAX_PATH;
    RegQueryValueEx(key, "diff", 0, NULL, (BYTE*)command, &hlen);

    RegCloseKey(key);
  }

  SetDlgItemText(hDlg, ID_DIFF_COMMAND, command);
  SendDlgItemMessage(hDlg, ID_DIFF_COMMAND, EM_SETLIMITTEXT, MAX_PATH, 0);
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

  	    GetDlgItemText(hwndDlg, ID_DIFF_COMMAND, command, MAX_PATH);

	    RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Z\\diff_ext\\", 0, KEY_SET_VALUE, &key);
	    RegSetValueEx(key, "diff", 0, REG_SZ, (const BYTE*)command, strlen(command));
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
	client.bottom = 85;
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
        WORD width = LOWORD(lParam);
        WORD height = HIWORD(lParam);
	RECT rect;
	int x;
	int y;
	int w;
	int h;

	HWND hwnd_item;

        hwnd_item = GetDlgItem(hwndDlg, ID_ABOUT);

	GetWindowRect(hwnd_item, &rect);

	rect.left = -45;
	rect.top = -20;
	rect.right = -5;
	rect.bottom = -5;

	MapDialogRect(hwndDlg, &rect);

	w = rect.right-rect.left;
	h = rect.bottom-rect.top;
	x = width+rect.left;
	y = height+rect.top;

	MoveWindow(hwnd_item, x, y, w, h, FALSE);

        hwnd_item = GetDlgItem(hwndDlg, IDCANCEL);

	rect.left = -90;
	rect.top = -20;
	rect.right = -50;
	rect.bottom = -5;

	MapDialogRect(hwndDlg, &rect);

	w = rect.right-rect.left;
	h = rect.bottom-rect.top;
	x = width+rect.left;
	y = height+rect.top;

	MoveWindow(hwnd_item, x, y, w, h, FALSE);

        hwnd_item = GetDlgItem(hwndDlg, IDOK);

	rect.left = -135;
	rect.top = -20;
	rect.right = -95;
	rect.bottom = -5;

	MapDialogRect(hwndDlg, &rect);

	w = rect.right-rect.left;
	h = rect.bottom-rect.top;
	x = width+rect.left;
	y = height+rect.top;

	MoveWindow(hwnd_item, x, y, w, h, FALSE);

        hwnd_item = GetDlgItem(hwndDlg, ID_GROUP);

	rect.left = 5;
	rect.top = 5;
	rect.right = -5;
	rect.bottom = 32;

	MapDialogRect(hwndDlg, &rect);

	w = width-(rect.left-rect.right);
	h = rect.bottom-rect.top;
	x = rect.left;
	y = rect.top;

	MoveWindow(hwnd_item, x, y, w, h, FALSE);

        hwnd_item = GetDlgItem(hwndDlg, ID_DIFF_COMMAND);

	rect.left = 10;
	rect.top = 15;
	rect.right = -55;
	rect.bottom = 27;

	MapDialogRect(hwndDlg, &rect);

	w = width-(rect.left-rect.right);
	h = rect.bottom-rect.top;
	x = rect.left;
	y = rect.top;

	MoveWindow(hwnd_item, x, y, w, h, FALSE);

        hwnd_item = GetDlgItem(hwndDlg, ID_BROWSE);

	rect.left = -50;
	rect.top = 15;
	rect.right = -10;
	rect.bottom = 27;

	MapDialogRect(hwndDlg, &rect);

	w = rect.right-rect.left;
	h = rect.bottom-rect.top;
	x = width+rect.left;
	y = rect.top;

	MoveWindow(hwnd_item, x, y, w, h, FALSE);

/**/
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
