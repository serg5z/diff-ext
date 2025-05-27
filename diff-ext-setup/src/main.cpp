/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <shellapi.h>

#include <string>
#include <format>

#include "settings.h"
#include "resource.h"


static SIZE g_originalDlgSize = { 0, 0 };
static RECT r_cancel, r_ok, r_browse, r_diff, r_about, r_resize;

extern "C" IMAGE_DOS_HEADER __ImageBase;

std::wstring
GetProductVersionString() {
    WCHAR modulePath[MAX_PATH] = {};
    GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH);

    DWORD dummy = 0;
    DWORD size = GetFileVersionInfoSizeW(modulePath, &dummy);
    if(!size) return L"";

    std::vector<BYTE>
    data(size);
    if(!GetFileVersionInfoW(modulePath, 0, size, data.data()))
        return L"";

    struct LANGANDCODEPAGE { WORD wLanguage; WORD wCodePage; };
    LANGANDCODEPAGE* lpTranslate = nullptr;
    UINT cbTranslate = 0;
    if(!VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate) || cbTranslate < sizeof(LANGANDCODEPAGE))
        return L"";

    wchar_t subBlock[128];
    swprintf_s(subBlock, L"\\StringFileInfo\\%04x%04x\\ProductVersion",
                lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);

    LPVOID lpBuffer = nullptr;
    UINT dwBytes = 0;
    if(VerQueryValueW(data.data(), subBlock, &lpBuffer, &dwBytes) && lpBuffer)
        return
        std::wstring((wchar_t*)lpBuffer);

    return L"";
}

std::wstring
GetCopyrightString() {
    WCHAR modulePath[MAX_PATH] = {};
    GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH);

    DWORD dummy = 0;
    DWORD size = GetFileVersionInfoSizeW(modulePath, &dummy);
    if(!size) return L"";

    std::vector<BYTE>
    data(size);
    if(!GetFileVersionInfoW(modulePath, 0, size, data.data()))
        return L"";

    struct LANGANDCODEPAGE { WORD wLanguage; WORD wCodePage; };
    LANGANDCODEPAGE* lpTranslate = nullptr;
    UINT cbTranslate = 0;
    if(!VerQueryValueW(data.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate) || cbTranslate < sizeof(LANGANDCODEPAGE))
        return L"";

    wchar_t subBlock[128];
    swprintf_s(subBlock, L"\\StringFileInfo\\%04x%04x\\LegalCopyright",
                lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);

    LPVOID lpBuffer = nullptr;
    UINT dwBytes = 0;
    if(VerQueryValueW(data.data(), subBlock, &lpBuffer, &dwBytes) && lpBuffer)
        return
        std::wstring((wchar_t*)lpBuffer);

    return L"";
}

static INT_PTR CALLBACK 
AboutDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_INITDIALOG: {
            std::wstring version = std::format(L"diff-ext {}", GetProductVersionString());
            SetDlgItemTextW(hwnd, IDC_VERSION, version.c_str());

            std::wstring copyright = GetCopyrightString();
            SetDlgItemTextW(hwnd, IDC_COPYRIGHT, copyright.c_str());

            HICON hIcon = (HICON)LoadImage(
                GetModuleHandle(nullptr),
                MAKEINTRESOURCE(IDI_MAIN_ICON),
                IMAGE_ICON,
                GetSystemMetrics(SM_CXSMICON),
                GetSystemMetrics(SM_CYSMICON),
                LR_DEFAULTCOLOR);

            if(hIcon) {
                SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            }

            return TRUE;
        }

        case WM_COMMAND:
            if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hwnd, LOWORD(wParam));
                return TRUE;
            }
            break;
    }

    return FALSE;
}

static INT_PTR CALLBACK 
DiffExtSetupDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_INITDIALOG: {
            RECT dialog;
            GetWindowRect(hwnd, &dialog);
            g_originalDlgSize.cx = dialog.right - dialog.left;
            g_originalDlgSize.cy = dialog.bottom - dialog.top;

            RECT rc;
            
            GetClientRect(hwnd, &rc);
            MapWindowPoints(hwnd, nullptr, (LPPOINT)&rc, 2);

            GetWindowRect(GetDlgItem(hwnd, IDCANCEL), &r_cancel);

            r_cancel.left -= rc.right;
            r_cancel.right -= rc.right;
            r_cancel.top -= rc.bottom;
            r_cancel.bottom -= rc.bottom;

            GetWindowRect(GetDlgItem(hwnd, IDOK), &r_ok);

            r_ok.left -= rc.right;
            r_ok.right -= rc.right;
            r_ok.top -= rc.bottom;
            r_ok.bottom -= rc.bottom;

            GetWindowRect(GetDlgItem(hwnd, IDC_BROWSE), &r_browse);

            r_browse.left -= rc.right;
            r_browse.right -= rc.right;
            r_browse.top -= rc.top;
            r_browse.bottom -= rc.top;

            GetWindowRect(GetDlgItem(hwnd, IDC_EDIT_DIFF), &r_diff);

            r_diff.left -= rc.left;
            r_diff.right -= rc.right;
            r_diff.top -= rc.top;
            r_diff.bottom -= rc.top;

            GetWindowRect(GetDlgItem(hwnd, IDC_ABOUT), &r_about);

            r_about.left -= rc.right;
            r_about.right -= rc.right;
            r_about.top -= rc.bottom;
            r_about.bottom -= rc.bottom;

            GetWindowRect(GetDlgItem(hwnd, IDC_RESIZE), &r_resize);

            r_resize.left -= rc.right;
            r_resize.right -= rc.right;
            r_resize.top -= rc.bottom;
            r_resize.bottom -= rc.bottom;            

            // Initialize values
            SetDlgItemTextW(hwnd, IDC_EDIT_DIFF, getDiffTool().c_str());
            SetDlgItemInt(hwnd, IDC_EDIT_MRU, getMRUCapacity(), FALSE);
            SendMessageW(GetDlgItem(hwnd, IDC_SPIN_MRU), UDM_SETRANGE, 0, MAKELPARAM(64, 1));

            HICON hIcon = (HICON)LoadImage(
                GetModuleHandle(nullptr),
                MAKEINTRESOURCE(IDI_MAIN_ICON),
                IMAGE_ICON,
                GetSystemMetrics(SM_CXSMICON),
                GetSystemMetrics(SM_CYSMICON),
                LR_DEFAULTCOLOR);

            if(hIcon) {
                SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            }

            return TRUE;
        }

        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;

            mmi->ptMinTrackSize.x = g_originalDlgSize.cx;
            mmi->ptMinTrackSize.y = g_originalDlgSize.cy;

            return 0;
        }

        case WM_SIZE: {
            RECT client;
            GetClientRect(hwnd, &client);
            int width = client.right;
            int height = client.bottom;

            HDWP position_handle = BeginDeferWindowPos(4);

            DeferWindowPos(position_handle, GetDlgItem(hwnd, IDCANCEL), 0,
                r_cancel.left + width,
                r_cancel.top + height,
                r_cancel.right - r_cancel.left,
                r_cancel.bottom - r_cancel.top,
                SWP_NOZORDER);

            DeferWindowPos(position_handle, GetDlgItem(hwnd, IDOK), 0,
                r_ok.left + width,
                r_ok.top + height,
                r_ok.right - r_ok.left,
                r_ok.bottom - r_ok.top,
                SWP_NOZORDER);

            DeferWindowPos(position_handle, GetDlgItem(hwnd, IDC_BROWSE), 0,
                r_browse.left + width,
                r_browse.top,
                r_browse.right - r_browse.left,
                r_browse.bottom - r_browse.top,
                SWP_NOZORDER);

            DeferWindowPos(position_handle, GetDlgItem(hwnd, IDC_ABOUT), 0,
                r_about.left + width,
                r_about.top + height,
                r_about.right - r_about.left,
                r_about.bottom - r_about.top,
                SWP_NOZORDER);

            DeferWindowPos(position_handle, GetDlgItem(hwnd, IDC_EDIT_DIFF), 0,
                r_diff.left,
                r_diff.top,
                width - r_diff.left + r_diff.right,
                r_diff.bottom - r_browse.top,
                SWP_NOZORDER);

            DeferWindowPos(position_handle, GetDlgItem(hwnd, IDC_RESIZE), 0,
                r_resize.left + width,
                r_resize.top + height,
                r_resize.right - r_resize.left,
                r_resize.bottom - r_resize.top,
                SWP_NOZORDER);

            EndDeferWindowPos(position_handle);

            return TRUE;
        }

        case WM_NOTIFY: {
            NMHDR* pnmh = (NMHDR*)lParam;
            if(pnmh->idFrom == IDC_SPIN_MRU && pnmh->code == UDN_DELTAPOS) {
                NMUPDOWN* pnmud = (NMUPDOWN*)pnmh;

                // Get current value
                BOOL success;
                int value = GetDlgItemInt(hwnd, IDC_EDIT_MRU, &success, FALSE);
                if(!success) value = 1;

                // Adjust value: note negation of iDelta to fix reversed control
                int newValue = value + pnmud->iDelta;

                // Clamp to range
                if(newValue < 1) newValue = 1;
                if(newValue > 64) newValue = 64;

                SetDlgItemInt(hwnd, IDC_EDIT_MRU, newValue, FALSE);

                // Mark message handled
                return TRUE;
            }
        }
        break;
        
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case 1003: { // Browse button
                    wchar_t path[MAX_PATH] = {};
                    OPENFILENAMEW ofn = {};
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = L"Executables\0*.exe\0All Files\0*.*\0";
                    ofn.lpstrFile = path;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_FILEMUSTEXIST;
                    if(GetOpenFileName(&ofn)) {
                        SetDlgItemText(hwnd, 1001, path);
                    }
                    return TRUE;
                }

                case IDC_ABOUT:
                    DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, AboutDialogProc, 0);
                    return TRUE;

                case IDOK: {
                    wchar_t buffer[MAX_PATH];
                    std::wstring selectedDiffTool;
                    
                    GetDlgItemText(hwnd, 1001, buffer, MAX_PATH);
                    selectedDiffTool = buffer;

                    BOOL success = FALSE;
                    UINT size = GetDlgItemInt(hwnd, 1002, &success, FALSE);
                    if(success && size > 0 && size <= 64) {
                        setMRUCapacity(size);
                        setDiffTool(selectedDiffTool);
                        SaveSettings();
                        EndDialog(hwnd, IDOK);
                    } else {
                        MessageBox(hwnd, L"Please enter a valid MRU size (1 - 64).", L"Invalid Input", MB_OK | MB_ICONWARNING);
                    }
                    return TRUE;
                }

                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
            }
    }
    
    return FALSE;
}

int WINAPI 
wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
    LoadSettings();

    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icex);

    DialogBoxParamW(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), nullptr, DiffExtSetupDialogProc, 0);
    return 0;
}
