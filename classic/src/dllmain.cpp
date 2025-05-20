#include <Windows.h>
#include <guiddef.h>
#include <shlobj_core.h>
#include <shobjidl.h>
#include <wrl/implements.h>

#include <string>

#include "settings.h"
#include "contextmenu.h"
#include "classfactory.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Details;

// {C42D835A-32CB-11F0-8E44-FAE5B572B91D}
static const CLSID CLSID_ClassicContextMenu = 
{ 0xc42d835a, 0x32cb, 0x11f0, { 0x8e, 0x44, 0xfa, 0xe5, 0xb5, 0x72, 0xb9, 0x1d } };

extern "C" HRESULT __stdcall 
DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    if(IsEqualCLSID(rclsid, CLSID_ClassicContextMenu)) {
        ComPtr<ClassFactory<ContextMenu>> factory = Make<ClassFactory<ContextMenu>>();
        return factory.CopyTo(riid, ppv);
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}

extern "C" HRESULT __stdcall 
DllCanUnloadNow() {
    return S_OK;
}

extern "C" IMAGE_DOS_HEADER __ImageBase;

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

BOOL APIENTRY 
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
    switch (ul_reason_for_call) {
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_ATTACH:
            OutputDebugStringW(L"DiffExt-classic: DLL_PROCESS_ATTACH\n");
            LoadSettings();
            break;
        case DLL_PROCESS_DETACH:
            OutputDebugStringW(L"DiffExt-classic: DLL_PROCESS_DETACH\n");
            SaveSettings();
            break;
    }

    return TRUE;
}