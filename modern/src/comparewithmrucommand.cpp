/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include "settings.h"
#include "util.h"
#include "resources.h"
#include "comparewithmrucommand.h"


extern "C" IMAGE_DOS_HEADER __ImageBase;

CompareWithMRUCommand::CompareWithMRUCommand(const std::wstring& title, ULONG index) 
: BaseCommand(title, L"Compare with MRU item"), _index(index) {
}

IFACEMETHODIMP
CompareWithMRUCommand::GetIcon(IShellItemArray*, LPWSTR* icon) {
    wchar_t modulePath[MAX_PATH];

    if(!GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), modulePath, MAX_PATH)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    wchar_t iconPath[MAX_PATH + 20];
    swprintf_s(iconPath, L"%s,-%d", modulePath, IDI_COMPARE_ICON);

    OutputDebugStringW((std::wstring(L"CompareWithMRUCommand::GetIcon: ") + iconPath).c_str());

    // *icon = ::SysAllocString(iconPath);
    // return (*icon) ? S_OK : E_OUTOFMEMORY;
    return SHStrDupW(iconPath, icon);

// SHFILEINFO sfi = { 0 };

// if(SHGetFileInfoW(getMRU()[_index].c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICONLOCATION | SHGFI_USEFILEATTRIBUTES)) {
//   std::wstring iconPath = L"%SystemRoot%\\System32\\shell32.dll,";
//   iconPath += std::to_wstring(sfi.iIcon);
//   return SHStrDupW(iconPath.c_str(), icon);
// }

// return SHStrDupW(L"%SystemRoot%\\System32\\shell32.dll,-1", icon);
}

IFACEMETHODIMP
CompareWithMRUCommand::Invoke(IShellItemArray* items, IBindCtx*) {
    DWORD count = 0;

    if(FAILED(items->GetCount(&count)) || count != 1)
        return E_INVALIDARG;

    auto mru = getMRU();

    if(_index < mru.size()) {
        ComPtr<IShellItem> item;
        items->GetItemAt(0, &item);

        PWSTR path1 = nullptr;
        item->GetDisplayName(SIGDN_FILESYSPATH, &path1);

        launch_diff_tool(path1, mru[_index]);

        CoTaskMemFree(path1);
    } else {
        return E_INVALIDARG;
    }

    return S_OK;
}

IFACEMETHODIMP
CompareWithMRUCommand::GetState(IShellItemArray* psiItemArray, BOOL, EXPCMDSTATE* pCmdState) {
    DWORD count;

    psiItemArray->GetCount(&count);
    *pCmdState = count == 1 ? ECS_ENABLED : ECS_HIDDEN;

    return S_OK;
}

// IFACEMETHODIMP 
// CompareWithMRUCommand::GetIcon(IShellItemArray*, LPWSTR* ppszIcon) {
// SHFILEINFO sfi = { 0 };

// if(SHGetFileInfoW(getMRU()[_index].c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICONLOCATION | SHGFI_USEFILEATTRIBUTES)) {
//   std::wstring iconPath = L"%SystemRoot%\\System32\\shell32.dll,";
//   iconPath += std::to_wstring(sfi.iIcon);
//   return SHStrDupW(iconPath.c_str(), ppszIcon);
// }

// return SHStrDupW(L"%SystemRoot%\\System32\\shell32.dll,-1", ppszIcon);
//   return SHStrDupW(L"", ppszIcon);
// }

ULONG
CompareWithMRUCommand::getIndex() const { 
    return _index; 
}
