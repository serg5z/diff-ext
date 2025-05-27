#include <windows.h>
#include <guiddef.h>
#include <shlobj_core.h>
#include <shobjidl.h>
#include <wrl/module.h> 
#include <wrl/implements.h>
#include <gdiplus.h>

#include <string>

#include "settings.h"
#include "contextmenu.h"
#include "resources.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;
using namespace Gdiplus;

HBITMAP compare_bitmap = nullptr;
HBITMAP remember_bitmap = nullptr;
HBITMAP clear_mru_bitmap = nullptr;

static ULONG_PTR g_gdiplusToken = 0;
static bool g_gdiplusInitialized = false;

extern "C" IMAGE_DOS_HEADER __ImageBase;

// {C42D835A-32CB-11F0-8E44-FAE5B572B91D}
static const CLSID CLSID_ClassicContextMenu = 
{ 0xc42d835a, 0x32cb, 0x11f0, { 0x8e, 0x44, 0xfa, 0xe5, 0xb5, 0x72, 0xb9, 0x1d } };

CoCreatableClass(ContextMenu);

extern "C" HRESULT __stdcall 
DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, ppv);
}

extern "C" HRESULT __stdcall 
DllCanUnloadNow() {
    return S_OK;
}

extern "C" HRESULT __stdcall 
DllRegisterServer() {
    wchar_t modulePath[MAX_PATH];

    if(!GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH))
        return HRESULT_FROM_WIN32(GetLastError());

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_ClassicContextMenu, clsidStr, ARRAYSIZE(clsidStr));

    // Register the CLSID under HKEY_CLASSES_ROOT\CLSID
    std::wstring clsidKeyPath = L"CLSID\\" + std::wstring(clsidStr);
    HKEY hClsidKey;
    if(RegCreateKeyExW(HKEY_CLASSES_ROOT, clsidKeyPath.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hClsidKey, nullptr) != ERROR_SUCCESS)
        return E_ACCESSDENIED;

    // Set the default value for the CLSID key
    std::wstring description = L"diff-ext Context Menu Extension";
    RegSetValueExW(hClsidKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(description.c_str()), static_cast<DWORD>((description.size() + 1) * sizeof(wchar_t)));

    // Create the InprocServer32 key
    HKEY hInprocKey;
    if(RegCreateKeyExW(hClsidKey, L"InprocServer32", 0, nullptr, 0, KEY_WRITE, nullptr, &hInprocKey, nullptr) == ERROR_SUCCESS) {
        // Set the default value to the module path
        RegSetValueExW(hInprocKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(modulePath), static_cast<DWORD>((wcslen(modulePath) + 1) * sizeof(wchar_t)));

        // Set the ThreadingModel value to "Apartment"
        std::wstring threadingModel = L"Apartment";
        RegSetValueExW(hInprocKey, L"ThreadingModel", 0, REG_SZ, reinterpret_cast<const BYTE*>(threadingModel.c_str()), static_cast<DWORD>((threadingModel.size() + 1) * sizeof(wchar_t)));

        RegCloseKey(hInprocKey);
    }

    RegCloseKey(hClsidKey);

    // Register the context menu handler under HKEY_CLASSES_ROOT\*\shellex\ContextMenuHandlers
    std::wstring handlerKeyPath = L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\ClassicContextMenu";
    HKEY hHandlerKey;
    if(RegCreateKeyExW(HKEY_LOCAL_MACHINE, handlerKeyPath.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hHandlerKey, nullptr) != ERROR_SUCCESS)
        return E_ACCESSDENIED;

    // Set the default value to the CLSID
    RegSetValueExW(hHandlerKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(clsidStr), static_cast<DWORD>((wcslen(clsidStr) + 1) * sizeof(wchar_t)));

    RegCloseKey(hHandlerKey);

    return S_OK;
}

extern "C" HRESULT __stdcall 
DllUnregisterServer() {
    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_ClassicContextMenu, clsidStr, ARRAYSIZE(clsidStr));

    // Remove the CLSID key
    std::wstring clsidKeyPath = L"CLSID\\" + std::wstring(clsidStr);
    if(RegDeleteTreeW(HKEY_CLASSES_ROOT, clsidKeyPath.c_str()) != ERROR_SUCCESS)
        return E_FAIL;

    // Remove the context menu handler key
    std::wstring handlerKeyPath = L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\ClassicContextMenu";
    if(RegDeleteTreeW(HKEY_LOCAL_MACHINE, handlerKeyPath.c_str()) != ERROR_SUCCESS)
        return E_FAIL;

    return S_OK;
}

HICON LoadIconFromDll(const std::wstring& dllPath, int iconIndex)
{
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

void 
EnsureGDIPlusStarted()
{
    if (!g_gdiplusInitialized) {
        Gdiplus::GdiplusStartupInput input;
        if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, nullptr) == Ok) {
            g_gdiplusInitialized = true;
        }
    }
}

void 
CleanupGDIPlus()
{
    if(g_gdiplusInitialized) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusInitialized = false;
    }
}

HBITMAP 
IconToBitmapGdiPlus(HICON hIcon, int width, int height) {
    HBITMAP result = nullptr;

    if(hIcon) {
        Bitmap* iconBitmap = Bitmap::FromHICON(hIcon);
        if(iconBitmap) {
            if (iconBitmap->GetLastStatus() == Ok) {
                Bitmap bmp(width, height, PixelFormat32bppARGB);
                Graphics g(&bmp);
                g.SetCompositingMode(CompositingModeSourceCopy);
                g.Clear(Color(0, 0, 0, 0));
                g.DrawImage(iconBitmap, 0, 0, width, height);

                HBITMAP hBitmap = nullptr;
                Color background(0, 0, 0, 0);

                if (bmp.GetHBITMAP(background, &hBitmap) == Ok) {
                    result = hBitmap;
                }
            }
            delete iconBitmap;
        }
    }

    return result;
}

BOOL APIENTRY 
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
    wchar_t modulePath[MAX_PATH];

    if(!GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH))
        return HRESULT_FROM_WIN32(GetLastError());

    switch (ul_reason_for_call) {
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_ATTACH: {
            LoadSettings();
#ifndef _M_ARM64
            int w = 32; //GetSystemMetrics(SM_CXSMICON);
            int h = 32; //GetSystemMetrics(SM_CYSMICON);

            EnsureGDIPlusStarted();

            HICON clear_mru_icon = LoadIconFromDll(L"%SystemRoot%\\System32\\shell32.dll", 32);
            if(clear_mru_icon) {
                clear_mru_bitmap = IconToBitmapGdiPlus(clear_mru_icon, w, h);
                DestroyIcon(clear_mru_icon);
            }

            HICON compare_icon = (HICON)LoadImageW(reinterpret_cast<HINSTANCE>(&__ImageBase),
                                     MAKEINTRESOURCEW(IDI_COMPARE_ICON),
                                     IMAGE_ICON,
                                     w,
                                     h,
                                     LR_DEFAULTCOLOR);
            if (compare_icon) {
                compare_bitmap = IconToBitmapGdiPlus(compare_icon, w, h);
                DestroyIcon(compare_icon);
            }

            HICON remember_icon = (HICON)LoadImageW(reinterpret_cast<HINSTANCE>(&__ImageBase),
                                     MAKEINTRESOURCEW(IDI_REMEMBER_ICON),
                                     IMAGE_ICON,
                                     w,
                                     h,
                                     LR_DEFAULTCOLOR);
            if (remember_icon) {
                remember_bitmap = IconToBitmapGdiPlus(remember_icon, w, h);
                DestroyIcon(remember_icon);
            }
#endif
            break;
        }
        case DLL_PROCESS_DETACH:
            SaveSettings();
#ifndef _M_ARM64
            DeleteObject(clear_mru_bitmap);
            DeleteObject(compare_bitmap);
            DeleteObject(remember_bitmap);
            CleanupGDIPlus();
#endif            
            break;
    }

    return TRUE;
}