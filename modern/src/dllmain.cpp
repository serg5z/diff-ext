#include <Windows.h>
#include <guiddef.h>
#include <shlobj_core.h>
#include <shobjidl.h>
#include <wrl/module.h>
#include <wrl/implements.h>

#include "settings.h"
#include "classfactory.h"
#include "comparefilescommand.h"
#include "rememberfilescommand.h"
#include "comparewithtopcommand.h"
#include "mrusubmenu.h"


// {C42D8356-32CB-11F0-8E44-FAE5B572B91D}
static const CLSID CLSID_CompareTwoFiles = 
{ 0xc42d8356, 0x32cb, 0x11f0, { 0x8e, 0x44, 0xfa, 0xe5, 0xb5, 0x72, 0xb9, 0x1d } };

// {C42D8357-32CB-11F0-8E44-FAE5B572B91D}
static const CLSID CLSID_RememberFiles = 
{ 0xc42d8357, 0x32cb, 0x11f0, { 0x8e, 0x44, 0xfa, 0xe5, 0xb5, 0x72, 0xb9, 0x1d } };

// {C42D8358-32CB-11F0-8E44-FAE5B572B91D}
static const CLSID CLSID_CompareWithTopMRU = 
{ 0xc42d8358, 0x32cb, 0x11f0, { 0x8e, 0x44, 0xfa, 0xe5, 0xb5, 0x72, 0xb9, 0x1d } };

// {C42D8359-32CB-11F0-8E44-FAE5B572B91D}
static const CLSID CLSID_MRUSubmenu = 
{ 0xc42d8359, 0x32cb, 0x11f0, { 0x8e, 0x44, 0xfa, 0xe5, 0xb5, 0x72, 0xb9, 0x1d } };

CoCreatableClass(CompareFilesCommand);
CoCreatableClass(RememberFilesCommand);
CoCreatableClass(CompareWithTopCommand);
CoCreatableClass(MRUSubmenu);

extern "C" HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, ppv);
}

/*
extern "C" HRESULT __stdcall 
DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (IsEqualCLSID(rclsid, CLSID_CompareTwoFiles))
        return Make<ClassFactory<CompareFilesCommand>>()->QueryInterface(riid, ppv);
    
    if (IsEqualCLSID(rclsid, CLSID_RememberFiles))
        return Make<ClassFactory<RememberFilesCommand>>()->QueryInterface(riid, ppv);
    
    if (IsEqualCLSID(rclsid, CLSID_CompareWithTopMRU))
        return Make<ClassFactory<CompareWithTopCommand>>()->QueryInterface(riid, ppv);

    if (IsEqualCLSID(rclsid, CLSID_MRUSubmenu))
        return Make<ClassFactory<MRUSubmenu>>()->QueryInterface(riid, ppv);

    return CLASS_E_CLASSNOTAVAILABLE;
}
*/

extern "C" HRESULT __stdcall 
DllCanUnloadNow() {
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
			OutputDebugStringW(L"DiffExt: DLL_PROCESS_ATTACH\n");
            LoadSettings();
            break;
        case DLL_PROCESS_DETACH:
			OutputDebugStringW(L"DiffExt: DLL_PROCESS_DETACH\n");
            SaveSettings();
            break;
    }

    return TRUE;
}
