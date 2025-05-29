/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include <windows.h>
#include <strsafe.h>
#include <shellapi.h> 
#include <commctrl.h>
#include <commoncontrols.h> 

#include <format>
#include <string>

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

extern HBITMAP compare_bitmap;
extern HBITMAP remember_bitmap;
extern HBITMAP clear_mru_bitmap;

extern HBITMAP IconToBitmap(HICON hIcon, int width, int height);

HBITMAP
GetFileTypeBitmap(const std::wstring path) {
    HBITMAP result = nullptr;
    SHFILEINFO sfi = { 0 };

    SHGetFileInfo(path.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);

    int iconIndex = sfi.iIcon;

    if(iconIndex >= 0) {
        IImageList* pImageList = nullptr;
        HRESULT hr = SHGetImageList(SHIL_SMALL, IID_IImageList, (void**)&pImageList);

        if(SUCCEEDED(hr) && pImageList) {
            IMAGEINFO imageInfo;
            HICON hIcon = nullptr;
            HRESULT hr = pImageList->GetIcon(iconIndex, ILD_TRANSPARENT, &hIcon);

            if(SUCCEEDED(hr) && hIcon) {
                int w = 32; //GetSystemMetrics(SM_CXSMICON);
                int h = 32; //GetSystemMetrics(SM_CYSMICON);

                HBITMAP bitmap = IconToBitmap(hIcon, w, h);
                result = bitmap;
            }
        }
    }

    return result;
}

void
insert_menu_item(HMENU menu, UINT id, UINT index, const wchar_t* text = nullptr, HBITMAP bitmap = nullptr, HMENU submenu = nullptr) {
    MENUITEMINFO mii = { sizeof(mii) };

    mii.fMask = MIIM_ID | MIIM_FTYPE;
    mii.wID = id;
    mii.fType = MFT_STRING;

    if (text) {
        mii.fMask |= MIIM_STRING;
        mii.dwTypeData = const_cast<LPWSTR>(text);
    }

    if (bitmap) {
        mii.fMask |= MIIM_BITMAP;
        mii.hbmpItem = bitmap;
    }

    if (submenu) {
        mii.fMask |= MIIM_SUBMENU;
        mii.hSubMenu = submenu;
    }

    InsertMenuItem(menu, index, TRUE, &mii);
}

ContextMenu::ContextMenu() {
}

ContextMenu::~ContextMenu() {
    _compare_to_items.clear();  
    
    if(_submenu) {
        DestroyMenu(_submenu);
    }
}

IFACEMETHODIMP
ContextMenu::Initialize(PCIDLIST_ABSOLUTE, IDataObject* pdtobj, HKEY) {
    _selected_files.clear();

    if(!pdtobj)
        return E_INVALIDARG;

    FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg;

    if(FAILED(pdtobj->GetData(&fmt, &stg)))
        return E_FAIL;

    HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);

    if(!hDrop) {
        ReleaseStgMedium(&stg);
        return E_FAIL;
    }

    UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
    for(UINT i = 0; i < fileCount; ++i) {
        wchar_t filePath[MAX_PATH];
        if(DragQueryFile(hDrop, i, filePath, MAX_PATH)) {
            _selected_files.emplace_back(filePath);
        }
    }

    GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);

    return S_OK;
}

IFACEMETHODIMP
ContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT, UINT uFlags) {
    ULONG result = 0;

    if(!(uFlags & CMF_DEFAULTONLY)) {
        InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);

        _compare_to_items.clear();
        
        if(_selected_files.size() == 1) {
            auto mru = getMRU();

            insert_menu_item(hMenu, idCmdFirst + IDM_REMEMBER, indexMenu++, L"Remember", remember_bitmap);

            if(!mru.empty()) {
                UINT id = idCmdFirst + IDM_COMPARE_TO_MRU_BASE;

                if(_submenu) {
                    DestroyMenu(_submenu);
                }
                
                _submenu = CreatePopupMenu();

                HBITMAP bitmap = GetFileTypeBitmap(mru[0]);
                auto str = std::make_unique<std::wstring>(std::format(L"Compare to {}", get_shortened_display_path(mru[0])));

                _compare_to_items.push_back(std::move(str));                        
                _compare_to_bitmaps.push_back(bitmap);

                insert_menu_item(hMenu, idCmdFirst + IDM_COMPARE_TO_MRU_TOP, indexMenu++, _compare_to_items.back()->data(), bitmap);

                size_t pos = 0;
                for(size_t i = 0; i < mru.size(); ++i, ++id, ++pos) {
                    auto str = std::make_unique<std::wstring>(get_shortened_display_path(mru[i]));
                    bitmap = GetFileTypeBitmap(mru[i]);

                    _compare_to_bitmaps.push_back(bitmap);
                    _compare_to_items.push_back(std::move(str)); 

                    insert_menu_item(_submenu, id, pos, _compare_to_items.back()->data(), bitmap);
                }

                pos++;
                AppendMenuW(_submenu, MF_SEPARATOR, 0, nullptr);

                insert_menu_item(_submenu, idCmdFirst + IDM_CLEAR_MRU, pos++, L"Clear MRU", clear_mru_bitmap);
                insert_menu_item(hMenu, idCmdFirst + IDM_CLEAR_MRU, indexMenu++, L"Compare to", compare_bitmap, _submenu);

                result = id - idCmdFirst + 1;
            } else {
                result = IDM_REMEMBER + 1;
            }
        } else if(_selected_files.size() == 2) {        
            insert_menu_item(hMenu, idCmdFirst + IDM_COMPARE, indexMenu++, L"Compare", compare_bitmap);
            insert_menu_item(hMenu, idCmdFirst + IDM_REMEMBER, indexMenu++, L"Remember", remember_bitmap);

            result = IDM_REMEMBER + 1;
        } else {
            insert_menu_item(hMenu, idCmdFirst + IDM_REMEMBER, indexMenu++, L"Remember", remember_bitmap);
            result = IDM_REMEMBER + 1;
        }
        InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    }

    return
    MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, result);
}

IFACEMETHODIMP
ContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
    OutputDebugStringW(L"InvokeCommand called\n");
    if(!pici) return E_POINTER;
    if(HIWORD(pici->lpVerb) != 0) return E_FAIL;

    int cmd = LOWORD(pici->lpVerb);
    auto mru = getMRU();

    wchar_t buffer[1024];
    switch(cmd) {
        case IDM_COMPARE:
            swprintf_s(buffer, L"comparing %s to %s\n", _selected_files[0].c_str(), _selected_files[1].c_str());
            OutputDebugStringW(buffer);

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
                swprintf_s(buffer, L"comparing to top %s to %s\n", _selected_files[0].c_str(), mru[0].c_str());
                OutputDebugStringW(buffer);
                launch_diff_tool(_selected_files[0], mru[0]);
            }
            break;

        default:
            if(cmd >= IDM_COMPARE_TO_MRU_BASE && cmd < IDM_COMPARE_TO_MRU_BASE + getMRUCapacity()) {
                int index = cmd - IDM_COMPARE_TO_MRU_BASE;

                if(index >= 0 && index < mru.size()) {
                    swprintf_s(buffer, L"comparing to index %s to %s\n", _selected_files[0].c_str(), mru[index].c_str());
                    launch_diff_tool(_selected_files[0], mru[index]);
                }
            }
            break;
    }
    return S_OK;
}

IFACEMETHODIMP
ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT*, LPSTR pszName, UINT cchMax) {
    if(uFlags & GCS_HELPTEXT) {
        if(idCmd == IDM_COMPARE) {
            StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, L"Compare the selected file.");
        } else if(idCmd == IDM_REMEMBER) {
            StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, L"Add the selected file to the MRU list.");
        } else if(idCmd == IDM_CLEAR_MRU) {
            StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, L"Clear the MRU list.");
        } else if(idCmd == IDM_COMPARE_TO_MRU_TOP) {
            auto mru = getMRU();

            if(!mru.empty()) {
                auto message = std::format(L"Compare {} to {}", _selected_files[0], mru[0]);
                StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, message.c_str());
            }
        } else if(idCmd >= IDM_COMPARE_TO_MRU_BASE && idCmd < IDM_COMPARE_TO_MRU_BASE + getMRUCapacity()) {
            int index = idCmd - IDM_COMPARE_TO_MRU_BASE;
            auto mru = getMRU();
            if(index >= 0 && index < mru.size()) {
                auto message = std::format(L"Compare {} to {}", _selected_files[0], mru[index]);
                StringCchCopyW(reinterpret_cast<LPWSTR>(pszName), cchMax, message.c_str());
            }
        }
    }

    return S_OK;
}
