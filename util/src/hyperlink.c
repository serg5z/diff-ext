/*
 * Module ID: hyperlink.cpp
 * Title    : CHyperLink definition.
 *
 * Author   : Olivier Langlois <olivier@olivierlanglois.net>
 * Date     : November 15, 2005
 *
 * To read the article describing this class, visit
 * http://www.olivierlanglois.net/hyperlinkdemo.htm
 *
 * Note: Strongly inspired by Neal Stublen code
 *       Minor ideas come from Chris Maunder and Paul DiLascia code
 *
 * Revision :
 *
 * 001        26-Nov-2005 - Olivier Langlois
 *            - Added changes to make CHyperLink compatible with UNICODE
 *            - Use dynamic memory allocation for the URL string
 *            - Use of the MAKEINTATOM macro
 */

#include <windows.h>
#include <tchar.h>

/*
 * Defines
 */
#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

typedef struct tagCHyperLink {
  COLORREF g_crLinkColor, g_crVisitedColor;
  HFONT    g_UnderlineFont;
  BOOL     m_bOverControl;
  BOOL     m_bVisited;
  HFONT    m_StdFont;
  WNDPROC  m_pfnOrigCtlProc;
  LPTSTR m_strURL;
} CHyperLink;

static LRESULT CALLBACK hyperlink_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK parent_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL setURL(CHyperLink* hl, LPCTSTR strURL);
void createLinkCursor();
void createUnderlineFont(CHyperLink* hl);

static ATOM atom;
static g_hLinkCursor = NULL;
int g_counter = 0;

CHyperLink*
new_hyperlink() {
  CHyperLink* hl = HeapAlloc(GetProcessHeap(), 0, sizeof(CHyperLink));
  
  atom = GlobalAddAtom(TEXT("\\{00e67b7f-4158-4c2a-8a80-44f5666fbeb7}"));
  
  hl->m_bOverControl = FALSE;
  hl->m_bVisited = FALSE;
  hl->m_StdFont = NULL;
  hl->m_pfnOrigCtlProc = NULL;
  hl->m_strURL = NULL;
  hl->g_crLinkColor = RGB(  0,   0, 255);
  hl->g_crVisitedColor = RGB( 128,  0, 128);
  hl->g_UnderlineFont = NULL;
  
  return hl;
}

/*-----------------------------------------------------------------------------
 * Public functions
 */

/*
 * Function CHyperLink::ConvertStaticToHyperlink
 */
BOOL 
set_url(HWND hwndCtl, LPCTSTR strURL) {
  CHyperLink* hl = new_hyperlink();
  DWORD dwStyle = GetWindowLong(hwndCtl, GWL_STYLE);
  BOOL result = FALSE;
  
  if(setURL(hl, strURL) == TRUE) {
    HWND hwndParent = GetParent(hwndCtl);
    
    result = TRUE;
    
    if(hwndParent != NULL) {
      WNDPROC pfnOrigProc = (WNDPROC)GetWindowLong(hwndParent, GWL_WNDPROC);
      if(GetProp(hwndParent, MAKEINTATOM(atom)) == NULL) {
        SetProp(hwndParent, MAKEINTATOM(atom), (HANDLE)pfnOrigProc);
        SetWindowLong(hwndParent, GWL_WNDPROC, (LONG) (WNDPROC)parent_procedure);
      }
    }

    SetWindowLong(hwndCtl, GWL_STYLE, dwStyle | SS_NOTIFY);

    if(GetProp(hwndCtl, MAKEINTATOM(atom)) == NULL) {
      hl->m_pfnOrigCtlProc = (WNDPROC)GetWindowLong(hwndCtl, GWL_WNDPROC);
      SetProp(hwndCtl, MAKEINTATOM(atom), (HANDLE)hl);
      SetWindowLong(hwndCtl, GWL_WNDPROC, (LONG)(WNDPROC)hyperlink_procedure);
      hl->m_StdFont = (HFONT)SendMessage(hwndCtl, WM_GETFONT, 0, 0);
    } else {
      CHyperLink* x = (CHyperLink*)GetProp(hwndCtl, MAKEINTATOM(atom));
      hl->m_pfnOrigCtlProc = x->m_pfnOrigCtlProc;
      hl->m_StdFont = x->m_StdFont;
    }
    
    createUnderlineFont(hl);
    createLinkCursor();
  }
  
  return TRUE;
}

/*
 * Function CHyperLink::ConvertStaticToHyperlink
 */
BOOL 
set_url3(HWND hwndParent, UINT uiCtlId, LPCTSTR strURL) {
  return set_url(GetDlgItem(hwndParent, uiCtlId), strURL);
}

/*
 * Function CHyperLink::setURL
 */
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

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Private functions
 */

/*
 * Function CHyperLink::parent_procedure
 */
LRESULT CALLBACK 
parent_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  WNDPROC pfnOrigProc = (WNDPROC)GetProp(hwnd, MAKEINTATOM(atom));
  LRESULT result;

  if(message == WM_CTLCOLORSTATIC) {
    HDC hdc = (HDC) wParam;
    HWND hwndCtl = (HWND) lParam;
    CHyperLink *pHyperLink = (CHyperLink *)GetProp(hwndCtl, MAKEINTATOM(atom));

    result = CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);

    if(pHyperLink) {
      if (!pHyperLink->m_bVisited) {
        SetTextColor(hdc, pHyperLink->g_crLinkColor);
      } else {
        SetTextColor(hdc, pHyperLink->g_crVisitedColor);
      }
    }
  } else if(message == WM_DESTROY) {
    SetWindowLong(hwnd, GWL_WNDPROC, (LONG) pfnOrigProc);
    RemoveProp(hwnd, MAKEINTATOM(atom));
    result = CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
  } else {
    result = CallWindowProc(pfnOrigProc, hwnd, message, wParam, lParam);
  }
  
  return result;
}

/*
 * Function CHyperLink::Navigate
 */
void 
Navigate(CHyperLink* hl) {
  SHELLEXECUTEINFO sei;
  ZeroMemory(&sei,sizeof(SHELLEXECUTEINFO));
  sei.cbSize = sizeof( SHELLEXECUTEINFO );
  sei.lpVerb = TEXT( "open" );
  sei.lpFile = hl->m_strURL;
  sei.nShow = SW_SHOWNORMAL;

  ShellExecuteEx(&sei);
  hl->m_bVisited = TRUE;
}

/*
 * Function CHyperLink::DrawFocusRect
 */
void 
draw_focus_rectangle(HWND hwnd) {
  HWND hwndParent = GetParent(hwnd);

  if(hwndParent != NULL) {
    RECT rc;
    GetWindowRect(hwnd, &rc);

    InflateRect(&rc,1,1);
    ScreenToClient(hwndParent, (LPPOINT)&rc);
    ScreenToClient(hwndParent, ((LPPOINT)&rc)+1);
    HDC dcParent = GetDC(hwndParent); 
    DrawFocusRect(dcParent, &rc);
    ReleaseDC(hwndParent,dcParent);
  }
}

/*
 * Function CHyperLink::hyperlink_procedure
 *
 * Note: Processed messages are not passed back to the static control
 *       procedure. It does work fine but be aware that it could cause
 *       some problems if the static control is already subclassed.
 *       Consider the example where the static control would be already
 *       subclassed with the ToolTip control that needs to process mouse
 *       messages. In that situation, the ToolTip control would not work
 *       as expected.
 */
LRESULT CALLBACK 
hyperlink_procedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  CHyperLink *pHyperLink = (CHyperLink *)GetProp(hwnd, MAKEINTATOM(atom));
  LRESULT result = 0;

  switch(message) {
    case WM_MOUSEMOVE:
      if(pHyperLink->m_bOverControl){
        RECT rect;
        POINT pt = {LOWORD(lParam), HIWORD(lParam)};

        GetClientRect(hwnd,&rect);

        if(!PtInRect(&rect,pt)) {
          ReleaseCapture();
        }
      } else {
        pHyperLink->m_bOverControl = TRUE;
        SendMessage(hwnd, WM_SETFONT, (WPARAM)pHyperLink->g_UnderlineFont, FALSE);
        InvalidateRect(hwnd, NULL, FALSE);
        SetCapture(hwnd);
      }
      break;
      
    case WM_SETCURSOR:
      SetCursor(g_hLinkCursor);
      result = TRUE;
      break;
    
    case WM_CAPTURECHANGED:
      pHyperLink->m_bOverControl = FALSE;
      SendMessage(hwnd, WM_SETFONT, (WPARAM)pHyperLink->m_StdFont, FALSE);
      InvalidateRect(hwnd, NULL, FALSE);
      break;

    case WM_KEYUP:
      if(wParam == VK_SPACE) {
        Navigate(pHyperLink);
      } else {
        result = CallWindowProc(pHyperLink->m_pfnOrigCtlProc, hwnd, message, wParam, lParam);
      }
      break;
  
    case WM_LBUTTONUP:
      Navigate(pHyperLink);
      break;
    
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
      draw_focus_rectangle(hwnd);
      break;
      
    case WM_DESTROY:
      SetWindowLong(hwnd, GWL_WNDPROC, (LONG)pHyperLink->m_pfnOrigCtlProc);

      SendMessage(hwnd, WM_SETFONT, (WPARAM)pHyperLink->m_StdFont, 0);

      if(--g_counter <= 0) {
        g_hLinkCursor   = NULL;
        DeleteObject(pHyperLink->g_UnderlineFont);
        pHyperLink->g_UnderlineFont = NULL;
      }

      RemoveProp(hwnd, MAKEINTATOM(atom));
      break;
      
    default: 
      result = CallWindowProc(pHyperLink->m_pfnOrigCtlProc, hwnd, message, wParam, lParam);
  }

  return result;
}

/*
 * Function CHyperLink::createUnderlineFont
 */
void 
createUnderlineFont(CHyperLink* hl) {
  LOGFONT lf;
  
  GetObject(hl->m_StdFont, sizeof(lf), &lf);
  lf.lfUnderline = TRUE;

  hl->g_UnderlineFont = CreateFontIndirect(&lf);
}

/*
 * Function CHyperLink::createLinkCursor
 */
void 
createLinkCursor() {
  if(g_hLinkCursor == NULL) {
    g_hLinkCursor = LoadCursor(NULL, IDC_HAND);
    if(g_hLinkCursor == NULL) {
      /*
       * There exist an alternative way to get the IDC_HAND by loading winhlp32.exe but I
       * estimated that it didn't worth the trouble as IDC_HAND is supported since Win98.
       * I consider that if a user is happy with 10 years old OS, he won't bother to have
       * an arrow cursor.
       */
      g_hLinkCursor = LoadCursor(NULL, IDC_ARROW);
    }
  }
}
 