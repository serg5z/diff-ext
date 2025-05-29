/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include <windows.h>
#include <guiddef.h>
#include <shlobj_core.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <wrl/module.h> 
#include <wrl/implements.h>

#include <string>
#include <format>

#include "settings.h"
#include "contextmenu.h"
#include "resources.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;

HBITMAP compare_bitmap = nullptr;
HBITMAP remember_bitmap = nullptr;
HBITMAP clear_mru_bitmap = nullptr;

static ULONG_PTR g_gdiplusToken = 0;
static bool g_gdiplusInitialized = false;

extern "C" IMAGE_DOS_HEADER __ImageBase;

// {C42D835A-32CB-11F0-8E44-FAE5B572B91D}
static const CLSID CLSID_ClassicContextMenu = 
{ 0xc42d835a, 0x32cb, 0x11f0, { 0x8e, 0x44, 0xfa, 0xe5, 0xb5, 0x72, 0xb9, 0x1d } };

#define EXT_DESCRIPTION  L"diff-ext Context Menu Extension"
#define INPROCSERVER32   L"InprocServer32"
#define THREADING_MODEL  L"Apartment"

CoCreatableClass(ContextMenu);

extern "C" HRESULT __stdcall 
DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    return
    Module<InProc>::GetModule().GetClassObject(rclsid, riid, ppv);
}

extern "C" HRESULT __stdcall 
DllCanUnloadNow() {
    return S_OK;
}

HICON
LoadIconFromDll(const std::wstring& dllPath, int iconIndex) {
    HICON hIconLarge = nullptr;
    int iconsExtracted = ExtractIconExW(
        dllPath.c_str(),  // Path to DLL or EXE
        iconIndex,        // Icon index (0-based, or negative for resource ID)
        &hIconLarge,      // Large icon
        nullptr,          // Small icon (optional)
        1                 // Number of icons to extract
    );

    if(iconsExtracted > 0 && hIconLarge != nullptr) {
        return hIconLarge;
    }

    return nullptr;
}

HBITMAP
IconToBitmap(HICON icon, int width, int height) {
    LPVOID bits;
    BITMAPINFO bmi = {0};

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC memory_dc = CreateCompatibleDC(NULL);
    HBITMAP bitmap = CreateDIBSection(NULL, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    if(bitmap) {
        HBITMAP prev_bitmap = (HBITMAP)SelectObject(memory_dc, bitmap);
        RECT rc = { 0, 0, width, height };
        if(bmi.bmiHeader.biBitCount <= 24) {
            SetBkColor(memory_dc, GetSysColor(COLOR_MENU));
            ExtTextOut(memory_dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
        }
        DrawIconEx(memory_dc, 0, 0, icon, width, height, 0, NULL, DI_NORMAL);
        SelectObject(memory_dc, prev_bitmap);
    }
    DeleteDC(memory_dc);

    return bitmap;
}

LONG 
SetRegKeyAndValue(HKEY hKeyRoot, LPCWSTR pszSubKey, LPCWSTR pszValueName, LPCWSTR pszData) {
    HKEY hKey;
    LONG lResult = RegCreateKeyExW(hKeyRoot, pszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

    if(lResult == ERROR_SUCCESS) {
        if(pszData) {
            lResult = RegSetValueExW(hKey, pszValueName, 0, REG_SZ, (const BYTE*)pszData, ((DWORD)wcslen(pszData) + 1) * sizeof(wchar_t));
        }

        RegCloseKey(hKey);
    }

    return lResult;
}

STDAPI 
DllRegisterServer() {
    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_ClassicContextMenu, clsidStr, ARRAYSIZE(clsidStr));

    // Add handler for all files
    SetRegKeyAndValue(HKEY_CLASSES_ROOT, L"*\\shellex\\ContextMenuHandlers\\diff-ext", NULL, clsidStr);

    // Add handler for directories
    SetRegKeyAndValue(HKEY_CLASSES_ROOT, L"Directory\\shellex\\ContextMenuHandlers\\diff-ext", NULL, clsidStr);

    SetRegKeyAndValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", clsidStr, EXT_DESCRIPTION);

    wchar_t modulePath[MAX_PATH];
    if(!GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH))
        return HRESULT_FROM_WIN32(GetLastError());

    wchar_t keyPath[128];
    HRESULT hr = StringCchPrintfW(keyPath, ARRAYSIZE(keyPath), L"CLSID\\%s", clsidStr);

    if(FAILED(hr)) 
        return hr;

    HKEY hKey = nullptr;

    if(RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return E_FAIL;

    RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)EXT_DESCRIPTION, sizeof(EXT_DESCRIPTION));
    HKEY hInproc = nullptr;

    if(RegCreateKeyExW(hKey, INPROCSERVER32, 0, NULL, 0, KEY_WRITE, NULL, &hInproc, NULL) == ERROR_SUCCESS) {
        RegSetValueExW(hInproc, NULL, 0, REG_SZ, (const BYTE*)modulePath, (DWORD)((wcslen(modulePath)+1)*sizeof(wchar_t)));
        RegSetValueExW(hInproc, L"ThreadingModel", 0, REG_SZ, (const BYTE*)THREADING_MODEL, sizeof(THREADING_MODEL));
        RegCloseKey(hInproc);
    }

    RegCloseKey(hKey);

    return S_OK;
}

STDAPI
DllUnregisterServer(){
    HRESULT result = S_OK;
    wchar_t clsidStr[48]; // {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
    StringFromGUID2(CLSID_ClassicContextMenu, clsidStr, ARRAYSIZE(clsidStr));

    // Remove handler for all files
    LONG res = SHDeleteKeyW(HKEY_CLASSES_ROOT, L"*\\shellex\\ContextMenuHandlers\\diff-ext");
    if(res != ERROR_SUCCESS && res != ERROR_FILE_NOT_FOUND && result == S_OK) {
        result = HRESULT_FROM_WIN32(res);
    }

    // Remove handler for directories
    res = SHDeleteKeyW(HKEY_CLASSES_ROOT, L"Directory\\shellex\\ContextMenuHandlers\\diff-ext");
    if(res != ERROR_SUCCESS && res != ERROR_FILE_NOT_FOUND && result == S_OK) {
        result = HRESULT_FROM_WIN32(res);
    }

    HKEY hKey = nullptr;
    res = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0, KEY_SET_VALUE, &hKey);
    if(res == ERROR_SUCCESS) {
        RegDeleteValueW(hKey, clsidStr);
        RegCloseKey(hKey);
    } else if(res != ERROR_FILE_NOT_FOUND && result == S_OK) {
        result = HRESULT_FROM_WIN32(res);
    }

    // Remove CLSID registration
    wchar_t key_path[48]; // 6 (CLSID\) + 38 (GUID) + 1 = 45
    HRESULT hr = StringCchPrintfW(key_path, ARRAYSIZE(key_path), L"CLSID\\%s", clsidStr);
    if(SUCCEEDED(hr)) {
        res = SHDeleteKeyW(HKEY_CLASSES_ROOT, key_path);
        if(res != ERROR_SUCCESS && res != ERROR_FILE_NOT_FOUND && result == S_OK) {
            result = HRESULT_FROM_WIN32(res);
        }
    } else if(result == S_OK) {
        result = hr;
    }

    return result;
}

BOOL APIENTRY 
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
    wchar_t modulePath[MAX_PATH];

    if(!GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH))
        return HRESULT_FROM_WIN32(GetLastError());

    switch(ul_reason_for_call) {
        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_ATTACH: {
            LoadSettings();
            int w = 32; //GetSystemMetrics(SM_CXSMICON);
            int h = 32; //GetSystemMetrics(SM_CYSMICON);

            HICON clear_mru_icon = LoadIconFromDll(L"%SystemRoot%\\System32\\shell32.dll", 32);
            if(clear_mru_icon) {
                clear_mru_bitmap = IconToBitmap(clear_mru_icon, w, h);
                DestroyIcon(clear_mru_icon);
            }

            HICON compare_icon = (HICON)LoadImageW(reinterpret_cast<HINSTANCE>(&__ImageBase),
                                    MAKEINTRESOURCEW(IDI_COMPARE_ICON),
                                    IMAGE_ICON,
                                    w,
                                    h,
                                    LR_DEFAULTCOLOR);
            if(compare_icon) {
                compare_bitmap = IconToBitmap(compare_icon, w, h);
                DestroyIcon(compare_icon);
            }

            HICON remember_icon = (HICON)LoadImageW(reinterpret_cast<HINSTANCE>(&__ImageBase),
                                    MAKEINTRESOURCEW(IDI_REMEMBER_ICON),
                                    IMAGE_ICON,
                                    w,
                                    h,
                                    LR_DEFAULTCOLOR);
            if(remember_icon) {
                remember_bitmap = IconToBitmap(remember_icon, w, h);
                DestroyIcon(remember_icon);
            }
            break;
        }

        case DLL_PROCESS_DETACH:
            SaveSettings();
            DeleteObject(clear_mru_bitmap);
            DeleteObject(compare_bitmap);
            DeleteObject(remember_bitmap);
            break;
    }

    return TRUE;
}