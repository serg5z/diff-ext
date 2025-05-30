/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include <wrl/client.h>

#include "settings.h"
#include "util.h"
#include "resources.h"
#include "comparefilescommand.h"
#include <string>


extern "C" IMAGE_DOS_HEADER __ImageBase;

CompareFilesCommand::CompareFilesCommand() : BaseCommand(L"Compare", L"Compare two selected files") {}

IFACEMETHODIMP
CompareFilesCommand::GetIcon(IShellItemArray*, LPWSTR* icon) {
    wchar_t modulePath[MAX_PATH];

    if(!GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH)) {
        return
        HRESULT_FROM_WIN32(GetLastError());
    }

    wchar_t iconPath[MAX_PATH + 20];
    swprintf_s(iconPath, L"%s,-%d", modulePath, IDI_COMPARE_ICON);

    return SHStrDupW(iconPath, icon);
}

IFACEMETHODIMP
CompareFilesCommand::Invoke(IShellItemArray* items, IBindCtx*) {
    DWORD count = 0;

    if(FAILED(items->GetCount(&count)) || count != 2)
        return E_INVALIDARG;

    ComPtr<IShellItem> item1, item2;
    items->GetItemAt(0, &item1);
    items->GetItemAt(1, &item2);

    PWSTR path1 = nullptr, path2 = nullptr;
    item1->GetDisplayName(SIGDN_FILESYSPATH, &path1);
    item2->GetDisplayName(SIGDN_FILESYSPATH, &path2);

    launch_diff_tool(path1, path2);

    CoTaskMemFree(path1);
    CoTaskMemFree(path2);

    return S_OK;
}

IFACEMETHODIMP
CompareFilesCommand::GetState(IShellItemArray* psiItemArray, BOOL, EXPCMDSTATE* pCmdState) {
    DWORD count;

    psiItemArray->GetCount(&count);
    *pCmdState = count == 2 ? ECS_ENABLED : ECS_HIDDEN;

    return S_OK;
}
