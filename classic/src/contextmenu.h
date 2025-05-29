/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#pragma once

#include <windows.h>
#include <shlobj_core.h>
#include <shobjidl.h>
#include <wrl/implements.h>
#include <string>
#include <vector>
#include <memory>


using namespace Microsoft::WRL;

class
__declspec(uuid("C42D835A-32CB-11F0-8E44-FAE5B572B91D")) 
ContextMenu : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IContextMenu, IShellExtInit> {
    public:
        ContextMenu();
        ~ContextMenu();

        // IShellExtInit
        IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID) override;

        // IContextMenu
        IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) override;
        IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici) override;
        IFACEMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax) override;

    private:
        HMENU _submenu = nullptr;
        std::vector<std::wstring> _selected_files;
        std::vector<std::unique_ptr<std::wstring>> _compare_to_items;
        std::vector<HBITMAP> _compare_to_bitmaps;
};

