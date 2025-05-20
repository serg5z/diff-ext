#include <strsafe.h>

#include <format>

#include "contextmenu.h"
#include "settings.h"
#include "util.h"

enum {
    IDM_COMPARE = 0,
    IDM_COMPARE_TO_MRU_TOP = 1,
    IDM_REMEMBER = 2,
    IDM_CLEAR_MRU = 3,
    IDM_COMPARE_TO_MRU_BASE = 10
};

ContextMenu::ContextMenu() {
    OutputDebugStringW(L"ContextMenu constructor called\n");
}

IFACEMETHODIMP 
ContextMenu::Initialize(PCIDLIST_ABSOLUTE, IDataObject* pdtobj, HKEY) {
    OutputDebugStringW(L"Initialize called\n");
    _selected_files.clear();

    if (!pdtobj)
        return E_INVALIDARG;

    FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg;

    if (FAILED(pdtobj->GetData(&fmt, &stg)))
        return E_FAIL;

    HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
    if (!hDrop) {
        ReleaseStgMedium(&stg);
        return E_FAIL;
    }

    UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
    for (UINT i = 0; i < fileCount; ++i) {
        wchar_t filePath[MAX_PATH];
        if (DragQueryFile(hDrop, i, filePath, MAX_PATH)) {
            _selected_files.emplace_back(filePath);
        }
    }

    GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);

    return S_OK;
}

IFACEMETHODIMP 
ContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT, UINT uFlags) {
    OutputDebugStringW(L"QueryContextMenu\n");

    if (uFlags & CMF_DEFAULTONLY)
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

    if(_selected_files.size() == 1) {
        auto mru = getMRU();

        InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION, idCmdFirst + IDM_REMEMBER, L"Remember");

        if(!mru.empty()) {
            UINT id = idCmdFirst + IDM_COMPARE_TO_MRU_BASE;

            InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION, idCmdFirst + IDM_COMPARE_TO_MRU_TOP, (L"Compare to " + get_shortened_display_path(mru[0])).c_str());

            HMENU hSubMenu = CreatePopupMenu();

            for (size_t i = 0; i < mru.size(); ++i, ++id) {
                AppendMenuW(hSubMenu, MF_STRING, id, get_shortened_display_path(mru[i]).c_str());
            }

            AppendMenuW(hSubMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hSubMenu, MF_STRING, idCmdFirst + IDM_CLEAR_MRU, L"Clear MRU");

            InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hSubMenu, L"Compare to");

            return MAKE_HRESULT(SEVERITY_SUCCESS, 0, id - idCmdFirst + 1);
        }

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, IDM_REMEMBER + 1);
    } else if(_selected_files.size() == 2) {
        InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION, idCmdFirst + IDM_COMPARE, L"Compare");
        InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION, idCmdFirst + IDM_REMEMBER, L"Remember");

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, IDM_REMEMBER + 1);
    } else {
        InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION, idCmdFirst + IDM_REMEMBER, L"Remember");
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, IDM_REMEMBER + 1);
    }
}

IFACEMETHODIMP 
ContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
    OutputDebugStringW(L"InvokeCommand called\n");
    if (!pici) return E_POINTER;
    if (HIWORD(pici->lpVerb) != 0) return E_FAIL;

    int cmd = LOWORD(pici->lpVerb);

    switch(cmd) {
        case IDM_COMPARE:
            launch_diff_tool(_selected_files[0], _selected_files[1]);
            break;
        case IDM_REMEMBER:
            for(const auto& file : _selected_files) {
                addToMRU(file);
            }
            SaveSettings();
            break;
        case IDM_CLEAR_MRU:
            clearMRU();
            SaveSettings();
            break;
        case IDM_COMPARE_TO_MRU_TOP: {
                auto mru = getMRU();
                launch_diff_tool(_selected_files[0], mru[0]);
            }
            break;
        default:
            if(cmd >= IDM_COMPARE_TO_MRU_BASE && cmd < IDM_COMPARE_TO_MRU_BASE + getMRUCapacity()) {
                int index = cmd - IDM_COMPARE_TO_MRU_BASE;
                auto mru = getMRU();
                if(index >= 0 && index < mru.size()) {
                    launch_diff_tool(_selected_files[0], mru[index]);
                }
            }
            break;
    }
    return S_OK;
}

IFACEMETHODIMP 
ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT*, LPSTR pszName, UINT cchMax) {
    if (uFlags & GCS_HELPTEXT) {
        if(idCmd == IDM_COMPARE) {
            StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, L"Compare the selected file.");
        }
        else if(idCmd == IDM_REMEMBER) {
            StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, L"Add the selected file to the MRU list.");
        }
        else if(idCmd == IDM_CLEAR_MRU) {
            StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, L"Clear the MRU list.");
        }
        else if(idCmd == IDM_COMPARE_TO_MRU_TOP) {
            auto mru = getMRU();
            if(!mru.empty()) {
                StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, std::format(L"Compare {} to {}", _selected_files[0], mru[0]).c_str());
            }
        } 
        else if(idCmd >= IDM_COMPARE_TO_MRU_BASE && idCmd < IDM_COMPARE_TO_MRU_BASE + getMRUCapacity()) {
            int index = idCmd - IDM_COMPARE_TO_MRU_BASE;
            auto mru = getMRU();
            if(index >= 0 && index < mru.size()) {
                StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, std::format(L"Compare {} to {}", _selected_files[0], mru[index]).c_str());
            }
        }
    }

    return S_OK;
}

