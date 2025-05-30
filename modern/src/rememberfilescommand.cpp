/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include "settings.h"
#include "resources.h"
#include "rememberfilescommand.h"


extern "C" IMAGE_DOS_HEADER __ImageBase;

RememberFilesCommand::RememberFilesCommand() : BaseCommand(L"Remember", L"Add selected files to MRU list") {
}

IFACEMETHODIMP
RememberFilesCommand::GetIcon(IShellItemArray*, LPWSTR* icon) {
    wchar_t modulePath[MAX_PATH];

    if(!GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH)) {
        return
        HRESULT_FROM_WIN32(GetLastError());
    }

    wchar_t iconPath[MAX_PATH + 20];
    swprintf_s(iconPath, L"%s,-%d", modulePath, IDI_REMEMBER_ICON);

    return SHStrDupW(iconPath, icon);
}

IFACEMETHODIMP
RememberFilesCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx*) {
    if(!psiItemArray) return E_INVALIDARG;

    DWORD count = 0;

    if(FAILED(psiItemArray->GetCount(&count)) || count == 0) return E_FAIL;

    for(DWORD i = 0; i < count; ++i) {
        ComPtr<IShellItem> item;

        if(SUCCEEDED(psiItemArray->GetItemAt(i, &item))) {
            PWSTR path = nullptr;
            if(SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
                addToMRU(path);
                CoTaskMemFree(path);
            }
        }
    }

    SaveSettings();

    return S_OK;
}
