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
#include "clearmrucommand.h"
#include "comparewithmrucommand.h"
#include "resources.h"
#include "mrusubmenu.h"


extern "C" IMAGE_DOS_HEADER __ImageBase;

MRUSubmenu::MRUSubmenu() {  
    const auto& mru = getMRU();

    size_t index = 0;
    for(const auto& file : mru) {
        _commands.push_back(Make<CompareWithMRUCommand>(index++));
    }

    if(!mru.empty()) {
        _commands.push_back(Make<ClearMRUCommand>());
    }
}

IFACEMETHODIMP
MRUSubmenu::GetTitle(IShellItemArray*, LPWSTR* ppszName) {
    return SHStrDupW(L"Compare to", ppszName);
}

IFACEMETHODIMP
MRUSubmenu::GetToolTip(IShellItemArray*, LPWSTR* ppszTip) {
    return SHStrDupW(L"List of previously remembered files", ppszTip);
}

IFACEMETHODIMP
MRUSubmenu::GetIcon(IShellItemArray*, LPWSTR* icon) {
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
MRUSubmenu::GetCanonicalName(GUID* pguidCommandName) {
    *pguidCommandName = GUID_NULL;

    return S_OK;
}


IFACEMETHODIMP
MRUSubmenu::GetFlags(EXPCMDFLAGS* pFlags) { 
    *pFlags = ECF_HASSUBCOMMANDS; 

    return S_OK; 
}

IFACEMETHODIMP
MRUSubmenu::EnumSubCommands(IEnumExplorerCommand** enumerator) {
    return QueryInterface(IID_PPV_ARGS(enumerator));
}

IFACEMETHODIMP
MRUSubmenu::Invoke(IShellItemArray*, IBindCtx*) { 
    return S_OK; 
}

IFACEMETHODIMP
MRUSubmenu::GetState(IShellItemArray* psiItemArray, BOOL, EXPCMDSTATE* pCmdState) {
    DWORD count;

    psiItemArray->GetCount(&count);
    *pCmdState = (count == 1 && !getMRU().empty()) ? ECS_ENABLED : ECS_HIDDEN;

    return S_OK;
}

IFACEMETHODIMP
MRUSubmenu::Next(ULONG celt, IExplorerCommand** rgelt, ULONG* pceltFetched) {
    ULONG count = 0;

    while(_current < _commands.size() && count < celt) {
        rgelt[count++] = _commands[_current++].Detach();
    }

    if(pceltFetched) {
        *pceltFetched = count;
    }

    return (count > 0) ? S_OK : S_FALSE;
}

IFACEMETHODIMP
MRUSubmenu::Skip(ULONG celt) {
    _current += celt;

    return (_current < _commands.size()) ? S_OK : S_FALSE;
}

IFACEMETHODIMP
MRUSubmenu::Reset() {
    _current = 0;

    return S_OK;
}

IFACEMETHODIMP
MRUSubmenu::Clone(IEnumExplorerCommand**) {
    return E_NOTIMPL;
}
