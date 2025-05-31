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

extern "C" {
    #include <layout.h>
}

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
            // Initialize control values
            SetDlgItemTextW(hwnd, IDC_EDIT_DIFF, getDiffTool().c_str());
            SetDlgItemInt(hwnd, IDC_EDIT_MRU, getMRUCapacity(), FALSE);
            SendMessageW(GetDlgItem(hwnd, IDC_SPIN_MRU), UDM_SETRANGE, 0, MAKELPARAM(64, 1));

            attach_layout(GetModuleHandle(nullptr), hwnd, MAKEINTRESOURCE(ID_MAINDIALOG_LAYOUT));

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
