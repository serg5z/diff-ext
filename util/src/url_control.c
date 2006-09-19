#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>
#include <util/dialog.h>

#define this_malloc(size) (void*)GlobalAlloc(GMEM_FIXED,(size))
#define this_free(ptr)    GlobalFree(ptr)

typedef struct {
  TCHAR szURL[MAX_PATH];
  WNDPROC oldproc;
  HFONT hfont;
  COLORREF crUnvisited;
  COLORREF crVisited;
  BOOL fClicking;
  DWORD dwFlags;
} URLCTRL;


BOOL util_url_draw(HWND,HDC,RECT*);
BOOL util_url_fit(HWND, BOOL fRedraw);
BOOL util_url_open(HWND);
LRESULT CALLBACK urlctrl_proc(HWND,UINT,WPARAM,LPARAM);

BOOL 
util_url_draw(HWND hwnd,HDC hdc,RECT *calcrect) {
  BOOL fResult=FALSE;
  
  if(GetWindowLong(hwnd,GWL_WNDPROC)==(LONG)urlctrl_proc && hdc) {
    URLCTRL *url=(URLCTRL*)GetWindowLong(hwnd,GWL_USERDATA);
    
    if(url) {
      HANDLE hOld;
      DWORD style=DT_SINGLELINE|DT_NOPREFIX;
      TCHAR szText[MAX_PATH];
      RECT rc;
      
      SendMessage(hwnd,WM_GETTEXT,(WPARAM)MAX_PATH,(LPARAM)szText);
      
      if(url->dwFlags&UCF_TXT_RIGHT) {
        style|=DT_RIGHT;
      } else if(url->dwFlags&UCF_TXT_HCENTER) {
        style|=DT_CENTER;
      }
      
      if(url->dwFlags&UCF_TXT_BOTTOM) {
        style|=DT_BOTTOM;
      } else if(url->dwFlags&UCF_TXT_VCENTER) {
        style|=DT_VCENTER;
      }
        
      if(calcrect==NULL) {
        COLORREF crOldText;
        int iOldBkMode;
        
        GetClientRect(hwnd,&rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_3DFACE+1));
        
        if(url->dwFlags&UCF_KBD) {
          if(GetFocus() == hwnd) {
            DrawFocusRect(hdc,&rc);
          }
          ++rc.left;
          --rc.right;
          ++rc.top;
          --rc.bottom;
        }
        
        crOldText = SetTextColor(hdc, (url->dwFlags & UCF_LNK_VISITED) ? (url->crVisited) : (url->crUnvisited));
        iOldBkMode = SetBkMode(hdc,TRANSPARENT);
        hOld = SelectObject(hdc, url->hfont);
        DrawText(hdc,szText,-1,&rc,style);
        SelectObject(hdc,hOld);
        SetBkMode(hdc,iOldBkMode);
        SetTextColor(hdc,crOldText);
      } else {
        calcrect->left = calcrect->top=0;
        calcrect->right = calcrect->bottom=0x00007FFF;
        hOld = SelectObject(hdc,url->hfont);
        DrawText(hdc,szText,-1,calcrect,style|DT_CALCRECT);
        SelectObject(hdc,hOld);
        
        if(url->dwFlags&UCF_KBD) {
          calcrect->right+=2;
          calcrect->bottom+=2;
        }
      }
      
      fResult=TRUE;
    }
  }
  
  return fResult;
}

BOOL 
util_url_fit(HWND hwnd, BOOL fRedraw) {
  BOOL fResult = FALSE;
  
  if(GetWindowLong(hwnd,GWL_WNDPROC)==(LONG)urlctrl_proc) {
    HDC hdc=GetDC(hwnd);
    
    if(hdc) {
      RECT rc;
      
      fResult = util_url_draw(hwnd, hdc, &rc);
      ReleaseDC(hwnd,hdc);
      
      if(fResult) {
        UINT flags = SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE;
        
        if(fRedraw == FALSE) {
          flags |= SWP_NOREDRAW;
        }
        
        fResult = SetWindowPos(hwnd, NULL, 0, 0, rc.right, rc.bottom, flags);
      }
    }
  }
  
  return fResult;
}

BOOL 
util_url_open(HWND hwnd) {
  BOOL fResult=FALSE;
  if(GetWindowLong(hwnd,GWL_WNDPROC)==(LONG)urlctrl_proc) {
    URLCTRL *url=(URLCTRL*)GetWindowLong(hwnd,GWL_USERDATA);
    
    if(url) {
      TCHAR *p,szText[MAX_PATH];
      
      if(url->szURL[0]) {
        p=url->szURL;
      } else {
        p=szText;
        SendMessage(hwnd,WM_GETTEXT,(WPARAM)MAX_PATH,(LPARAM)szText);
      }

      if((int)ShellExecute(NULL,_T("open"),p,NULL,NULL,SW_SHOWNORMAL)>32) {
        url->dwFlags|=UCF_LNK_VISITED;
        InvalidateRect(hwnd,NULL,TRUE);
        UpdateWindow(hwnd);
        fResult=TRUE;
      }
    }
  }
  return fResult;
}

LRESULT 
CALLBACK urlctrl_proc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
  URLCTRL *url=(URLCTRL*)GetWindowLong(hwnd,GWL_USERDATA);
  WNDPROC oldproc=url->oldproc;
  switch(uMsg) {
    case WM_NCDESTROY:
      SetWindowLong(hwnd,GWL_WNDPROC,(LONG)oldproc);
      
      if(url->hfont) {
        DeleteObject(url->hfont);
      }
        
      this_free(url);
      break;

    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc=(HDC)wParam;
      
      if(wParam==0) {
        hdc=BeginPaint(hwnd,&ps);
      }
      
      if(hdc) {
        util_url_draw(hwnd,hdc,NULL);
        if(wParam==0) EndPaint(hwnd,&ps);
      }
      return 0;
    }
    
    case WM_SETTEXT: {
      LRESULT result=CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
      if(url->dwFlags&UCF_FIT) util_url_fit(hwnd,FALSE);
      InvalidateRect(hwnd,NULL,TRUE);
      UpdateWindow(hwnd);
      return result;
    }
    
    case WM_SETFONT: {
      LOGFONT lf;
      HFONT hfont=(HFONT)wParam;
      
      if(hfont==NULL) {
        hfont=(HFONT)GetStockObject(SYSTEM_FONT);
      }
      
      GetObject(hfont,sizeof(LOGFONT),&lf);
      lf.lfUnderline = TRUE;
      
      if(url->hfont) {
        DeleteObject(url->hfont);
      }
      
      url->hfont=CreateFontIndirect(&lf);
      CallWindowProc(oldproc, hwnd, uMsg, wParam, 0);
      
      if(url->dwFlags & UCF_FIT) {
        util_url_fit(hwnd, FALSE);
      }
      
      if(LOWORD(lParam)) {
        InvalidateRect(hwnd,NULL,TRUE);
        UpdateWindow(hwnd);
      }
      return 0;
    }
    
    case WM_SETCURSOR: {
      SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_HAND)));
      return 1;
    }
    
    case WM_NCHITTEST: {
      return HTCLIENT;
    }
    
    case WM_LBUTTONDOWN: {
      if(url->dwFlags&UCF_KBD) SetFocus(hwnd);
      SetCapture(hwnd);
      url->fClicking=TRUE;
      break;
    }
    
    case WM_LBUTTONUP: {
      ReleaseCapture();
      if(url->fClicking) {
        POINT pt;
        RECT rc;
        url->fClicking=FALSE;
        pt.x=(short)LOWORD(lParam);
        pt.y=(short)HIWORD(lParam);
        ClientToScreen(hwnd,&pt);
        GetWindowRect(hwnd,&rc);
        if(PtInRect(&rc,pt)) util_url_open(hwnd);
      }
      break;
    }
    
    case WM_KEYDOWN: {
      if(url->dwFlags&UCF_KBD) {
        if(wParam==VK_SPACE) {
          util_url_open(hwnd);
          return 0;
        }
      }
      break;
    }
    
    case WM_KEYUP: {
      if(url->dwFlags&UCF_KBD) {
        if(wParam==VK_SPACE) {
          return 0;
        }
      }
      break;
    }
    
    case WM_GETDLGCODE: {
      if(url->dwFlags&UCF_KBD) return DLGC_WANTCHARS;
      break;
    }
    
    case WM_SETFOCUS:
    case WM_KILLFOCUS: 
      if(url->dwFlags&UCF_KBD) {
        InvalidateRect(hwnd,NULL,TRUE);
        UpdateWindow(hwnd);
        return 0;
      }
      break;
  }
  
  return CallWindowProc(oldproc,hwnd,uMsg,wParam,lParam);
}

BOOL 
urlctrl_set(HWND hwnd,TCHAR *szURL,COLORREF *unvisited,COLORREF *visited,DWORD dwFlags) {
  BOOL fResult=FALSE;
  
  if(IsWindow(hwnd)) {
    URLCTRL *url;
    
    if(GetWindowLong(hwnd,GWL_WNDPROC)==(LONG)urlctrl_proc) {
      url=(URLCTRL*)GetWindowLong(hwnd,GWL_USERDATA);
    } else {
      url=(URLCTRL*)this_malloc(sizeof(URLCTRL));
      
      if(url) {
        HFONT hfont=(HFONT)SendMessage(hwnd,WM_GETFONT,0,0);
        url->oldproc=(WNDPROC)SetWindowLong(hwnd,GWL_WNDPROC,(LONG)urlctrl_proc);
        SetWindowLong(hwnd,GWL_USERDATA,(LONG)url);
        url->fClicking = FALSE;
        SendMessage(hwnd,WM_SETFONT,(WPARAM)hfont,0);
      }
    }
    
    if(url) {
      LONG style = GetWindowLong(hwnd,GWL_STYLE);
      
      if(szURL) {
        lstrcpyn(url->szURL,szURL,MAX_PATH);
      } else {
        url->szURL[0] = 0;
      }
      
      url->crUnvisited = (unvisited) ? (*unvisited) : (RGB(0,0,255));
      url->crVisited = (visited) ? (*visited) : (RGB(128,0,128));


      url->dwFlags=dwFlags;
      style &= ~(WS_BORDER|WS_TABSTOP);
      style |= SS_NOTIFY;
      
      if(url->dwFlags&UCF_KBD) {
        style |= WS_TABSTOP;
      }
      
      SetWindowLong(hwnd,GWL_STYLE,style);
      
      if(url->dwFlags&UCF_FIT) {
        util_url_fit(hwnd, FALSE);
      }
      
      InvalidateRect(hwnd,NULL,TRUE);
      UpdateWindow(hwnd);
      fResult = TRUE;
    }
  }
  return fResult;
}

BOOL
urlctrl_fit(HWND hwnd) {
  return util_url_fit(hwnd, TRUE);
}
