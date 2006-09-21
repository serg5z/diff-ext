#include <windows.h>
#include <tchar.h>

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

typedef struct tagCHyperLink {
  COLORREF g_crLinkColor, g_crVisitedColor;
  HFONT g_UnderlineFont;
  BOOL m_bOverControl;
  BOOL m_bVisited;
  HFONT m_StdFont;
  WNDPROC m_pfnOrigCtlProc, m_pfnOrigProc;
  LPTSTR m_strURL;
} CHyperLink;

static LRESULT CALLBACK hyperlink_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK parent_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL setURL(CHyperLink* hl, LPCTSTR strURL);
void createLinkCursor();
HFONT createUnderlineFont(HFONT hl);

static ATOM atom;
static HCURSOR g_hLinkCursor = NULL;

CHyperLink*
new_hyperlink() {
  CHyperLink* hl = HeapAlloc(GetProcessHeap(), 0, sizeof(CHyperLink));
  
  atom = GlobalAddAtom(TEXT("\\{00e67b7f-4158-4c2a-8a80-44f5666fbeb7}"));
  
  hl->m_bOverControl = FALSE;
  hl->m_bVisited = FALSE;
  hl->m_StdFont = NULL;
  hl->m_pfnOrigCtlProc = (WNDPROC)NULL;
  hl->m_pfnOrigProc = (WNDPROC)NULL;
  hl->m_strURL = NULL;
  hl->g_crLinkColor = RGB(0, 0, 255);
  hl->g_crVisitedColor = RGB(128, 0, 128);
  hl->g_UnderlineFont = NULL;
  /*
HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\Settings\ -- link colors
  */
  return hl;
}

BOOL 
set_url(HWND hwndCtl, LPCTSTR strURL) {
  CHyperLink* hl = new_hyperlink();
  DWORD dwStyle = (DWORD)GetWindowLongPtr(hwndCtl, GWL_STYLE);
  BOOL result = FALSE;
  
  if(setURL(hl, strURL) == TRUE) {
    HWND hwndParent = GetParent(hwndCtl);
    
    result = TRUE;
    
    if(hwndParent != NULL) {
      hl->m_pfnOrigProc = (WNDPROC)GetWindowLongPtr(hwndParent, GWLP_WNDPROC);
      if(GetProp(hwndParent, MAKEINTATOM(atom)) == NULL) {
        SetProp(hwndParent, MAKEINTATOM(atom), (HANDLE)hl);
        SetWindowLongPtr(hwndParent, GWLP_WNDPROC, (LONG_PTR)parent_procedure);
      }
    }

    SetWindowLong(hwndCtl, GWL_STYLE, dwStyle | SS_NOTIFY);

    if(GetProp(hwndCtl, MAKEINTATOM(atom)) == NULL) {
      hl->m_pfnOrigCtlProc = (WNDPROC)GetWindowLongPtr(hwndCtl, GWLP_WNDPROC);
      SetProp(hwndCtl, MAKEINTATOM(atom), (HANDLE)hl);
      SetWindowLongPtr(hwndCtl, GWLP_WNDPROC, (LONG_PTR)hyperlink_procedure);
      hl->m_StdFont = (HFONT)SendMessage(hwndCtl, WM_GETFONT, 0, 0);
      hl->g_UnderlineFont = createUnderlineFont(hl->m_StdFont);
      SendMessage(hwndCtl, WM_SETFONT, (WPARAM)hl->g_UnderlineFont, FALSE);
    } else {
      CHyperLink* x = (CHyperLink*)GetProp(hwndCtl, MAKEINTATOM(atom));
      hl->m_pfnOrigCtlProc = x->m_pfnOrigCtlProc;
      hl->m_StdFont = x->m_StdFont;
      hl->g_UnderlineFont = x->g_UnderlineFont;
    }
    
    createLinkCursor();
  }
  
  return TRUE;
}

BOOL 
set_url3(HWND hwndParent, UINT uiCtlId, LPCTSTR strURL) {
  return set_url(GetDlgItem(hwndParent, uiCtlId), strURL);
}

BOOL 
setURL(CHyperLink* hl, LPCTSTR strURL) {
  BOOL result = TRUE;
  
  if(hl->m_strURL != NULL) {
    HeapFree(GetProcessHeap(), 0, hl->m_strURL);
  }
  
  hl->m_strURL = HeapAlloc(GetProcessHeap(), 0, (lstrlen(strURL)+1)*sizeof(TCHAR));
  
  if(hl->m_strURL != 0){
    lstrcpy(hl->m_strURL, strURL);
  } else {
    result = FALSE;
  }
  
  return result;
}

LRESULT CALLBACK 
parent_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  CHyperLink* hl = (CHyperLink*)GetProp(hwnd, MAKEINTATOM(atom));
  LRESULT result;

  if(message == WM_DESTROY) {
    RemoveProp(hwnd, MAKEINTATOM(atom));
    
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)hl->m_pfnOrigProc);
  }
  
  result = CallWindowProc(hl->m_pfnOrigProc, hwnd, message, wParam, lParam);
  
  if(message == WM_CTLCOLORSTATIC) {
    HDC hdc = (HDC) wParam;
    HWND hwndCtl = (HWND) lParam;
    CHyperLink* hl2 = (CHyperLink*)GetProp(hwndCtl, MAKEINTATOM(atom));

    if(hl2 != NULL) { 
      if(hl2->m_bVisited) {
        SetTextColor(hdc, hl2->g_crVisitedColor);
      } else {
        SetTextColor(hdc, hl2->g_crLinkColor);
      }
    }
  }
  
  return result;
}

void 
Navigate(CHyperLink* hl) {
  SHELLEXECUTEINFO sei;
  
  ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
  
  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  sei.lpVerb = TEXT("open");
  sei.lpFile = hl->m_strURL;
  sei.nShow = SW_SHOWNORMAL;

  ShellExecuteEx(&sei);
  hl->m_bVisited = TRUE;
}

void 
draw_focus_rectangle(HWND hwnd) {
  HWND hwndParent = GetParent(hwnd);

  if(hwndParent != NULL) {
    RECT rc;
    HDC dcParent = GetDC(hwndParent); 
    
    GetWindowRect(hwnd, &rc);

    InflateRect(&rc,1,1);
    ScreenToClient(hwndParent, (LPPOINT)&rc);
    ScreenToClient(hwndParent, ((LPPOINT)&rc)+1);
    DrawFocusRect(dcParent, &rc);
    ReleaseDC(hwndParent,dcParent);
  }
}

LRESULT CALLBACK 
hyperlink_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  CHyperLink* pHyperLink = (CHyperLink *)GetProp(hwnd, MAKEINTATOM(atom));
  LRESULT result = 0;

  switch(message) {
    case WM_LBUTTONDOWN:
      SetCapture(hwnd);
      pHyperLink->m_bOverControl = TRUE;
      break;
    
    case WM_LBUTTONUP: {
        POINT pt;
        RECT rect;
      
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        GetClientRect(hwnd,&rect);
      
        ReleaseCapture();
      
        if(PtInRect(&rect,pt)) {
          if(pHyperLink->m_bOverControl == TRUE) {
            Navigate(pHyperLink);
          }
        }
        
        pHyperLink->m_bOverControl = FALSE;
      }
      break;
    
    case WM_SETCURSOR:
      SetCursor(g_hLinkCursor);
      result = TRUE;
      break;
    
    case WM_KEYUP:
      if(wParam == VK_SPACE) {
        Navigate(pHyperLink);
      } else {
        result = CallWindowProc(pHyperLink->m_pfnOrigCtlProc, hwnd, message, wParam, lParam);
      }
      break;
  
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
      draw_focus_rectangle(hwnd);
      break;
      
    case WM_DESTROY: {
        HFONT uf = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
        WNDPROC original_procedure = pHyperLink->m_pfnOrigCtlProc;
      
        DeleteObject(uf);
        SendMessage(hwnd, WM_SETFONT, (WPARAM)pHyperLink->m_StdFont, 0);

        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)original_procedure);
      
        RemoveProp(hwnd, MAKEINTATOM(atom));
        HeapFree(GetProcessHeap(), 0, pHyperLink);
      
        result = CallWindowProc(original_procedure, hwnd, message, wParam, lParam);
      }
      break;
      
    default: 
      result = CallWindowProc(pHyperLink->m_pfnOrigCtlProc, hwnd, message, wParam, lParam);
  }

  return result;
}

HFONT 
createUnderlineFont(HFONT font) {
  LOGFONT lf;
  
  GetObject(font, sizeof(lf), &lf);
  lf.lfUnderline = TRUE;

  return CreateFontIndirect(&lf);
}

void 
createLinkCursor() {
  if(g_hLinkCursor == NULL) {
    g_hLinkCursor = LoadCursor(NULL, IDC_HAND);
    if(g_hLinkCursor == NULL) {
      g_hLinkCursor = LoadCursor(NULL, IDC_ARROW);
    }
  }
}
