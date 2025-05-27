/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include <Windows.h>
#include <guiddef.h>
#include <shlobj_core.h>
#include <shobjidl.h>
#include <wrl/module.h>
#include <wrl/implements.h>

#include "settings.h"
#include "comparefilescommand.h"
#include "rememberfilescommand.h"
#include "comparewithtopcommand.h"
#include "mrusubmenu.h"


CoCreatableClass(CompareFilesCommand);
CoCreatableClass(RememberFilesCommand);
CoCreatableClass(CompareWithTopCommand);
CoCreatableClass(MRUSubmenu);

extern "C" HRESULT __stdcall 
DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, ppv);
}

extern "C" HRESULT __stdcall 
DllCanUnloadNow() {
    return S_OK;
}

BOOL APIENTRY 
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID) {
    switch(ul_reason_for_call) {
        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_ATTACH:
            LoadSettings();
            break;

        case DLL_PROCESS_DETACH:
            SaveSettings();
            break;
    }

    return TRUE;
}
