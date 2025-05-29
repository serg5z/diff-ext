/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include <filesystem>
#include <string>


#include "settings.h"
#include "util.h"
#include "resources.h"
#include "comparewithmrucommand.h"


namespace fs = std::filesystem;

extern "C" IMAGE_DOS_HEADER __ImageBase;

CompareWithMRUCommand::CompareWithMRUCommand(ULONG index) 
: BaseCommand(L"", L"Compare with MRU item"), _index(index) {
    auto mru = getMRU();

    if(_index < mru.size()) {
        _title = (L"Compare to " + get_shortened_display_path(mru[_index])).c_str();
    }
}

IFACEMETHODIMP
CompareWithMRUCommand::GetIcon(IShellItemArray*, LPWSTR* icon) {
     SHFILEINFO sfi = { 0 };
    auto mru = getMRU();
    if(SHGetFileInfoW(mru[_index].c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICONLOCATION | SHGFI_USEFILEATTRIBUTES)) {
        fs::path p = mru[_index];
        std::wstring ext = p.extension();
        wchar_t progid[256] = L"";
        DWORD progid_size = sizeof(progid);

        OutputDebugStringW(ext.c_str());

        if(ext == L"") {
            DWORD attr = GetFileAttributesW(mru[_index].c_str());
            if((attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
                wchar_t iconpath[512] = L"";
                DWORD size = sizeof(iconpath);
                LONG res = SHGetValueW(
                    HKEY_CLASSES_ROOT,
                    L"Directory\\DefaultIcon",
                    nullptr,
                    nullptr,
                    iconpath,
                    &size
                );

                if(res == ERROR_SUCCESS) {
                    OutputDebugStringW(iconpath);
                    return SHStrDupW(iconpath, icon);
                }
                // fallback: hard-coded standard folder icon, just in case
                return SHStrDupW(L"%SystemRoot%\\System32\\shell32.dll,3", icon);
            }
        }

        if(SHGetValueW(HKEY_CLASSES_ROOT, ext.c_str(), nullptr, nullptr, progid, &progid_size) == ERROR_SUCCESS) {
            std::wstring key = std::wstring(progid) + L"\\DefaultIcon";
            wchar_t iconpath[512] = L"";
            DWORD iconpath_size = sizeof(progid);

            if (SHGetValueW(HKEY_CLASSES_ROOT, key.c_str(), nullptr, nullptr, iconpath, &iconpath_size) == ERROR_SUCCESS) {
                if ((wcscmp(iconpath, L"%1") == 0) || (wcscmp(iconpath,  L"\"%1\"") == 0)) {
                    std::wstring path = mru[_index] + L",0";
                    OutputDebugStringW((L"self1: " + path).c_str());
                    return SHStrDupW(path.c_str(), icon);
                }

                OutputDebugStringW(iconpath);
                return SHStrDupW(iconpath, icon);
            } else {
                HKEY hExtKey = NULL;
                if(RegOpenKeyExW(HKEY_CLASSES_ROOT, ext.c_str(), 0, KEY_READ, &hExtKey) == ERROR_SUCCESS) {
                    HKEY hOWP = NULL;
                    if(RegOpenKeyExW(hExtKey, L"OpenWithProgids", 0, KEY_READ, &hOWP) == ERROR_SUCCESS) {
                        wchar_t progid[256];
                        DWORD index = 0;
                        DWORD progidLen = ARRAYSIZE(progid);
                        LONG res;
                        // Enumerate first ProgID
                        while(true) {
                            progidLen = ARRAYSIZE(progid);
                            res = RegEnumValueW(hOWP, index, progid, &progidLen, NULL, NULL, NULL, NULL);
                            if(res == ERROR_NO_MORE_ITEMS) break;
                            if(res == ERROR_SUCCESS) {
                                std::wstring key = std::wstring(progid) + L"\\DefaultIcon";
                                wchar_t iconpath[512] = L"";
                                DWORD iconpath_size = sizeof(progid);

                                if(SHGetValueW(HKEY_CLASSES_ROOT, key.c_str(), nullptr, nullptr, iconpath, &iconpath_size) == ERROR_SUCCESS) {                    
                                    OutputDebugStringW(iconpath);
                                    return SHStrDupW(iconpath, icon);
                                } else {
                                    OutputDebugStringW(L"SHGetValueW 3 failed");
                                }
                            }
                            ++index;
                        }
                        RegCloseKey(hOWP);
                    }
                    RegCloseKey(hExtKey);
                }
            }
        } else {
            HKEY hExtKey = NULL;
            if(RegOpenKeyExW(HKEY_CLASSES_ROOT, ext.c_str(), 0, KEY_READ, &hExtKey) == ERROR_SUCCESS) {
                HKEY hOWP = NULL;
                if(RegOpenKeyExW(hExtKey, L"OpenWithProgids", 0, KEY_READ, &hOWP) == ERROR_SUCCESS) {
                    wchar_t progid[256];
                    DWORD index = 0;
                    DWORD progidLen = ARRAYSIZE(progid);
                    LONG res;
                    // Enumerate first ProgID
                    while(true) {
                        progidLen = ARRAYSIZE(progid);
                        res = RegEnumValueW(hOWP, index, progid, &progidLen, NULL, NULL, NULL, NULL);
                        if(res == ERROR_NO_MORE_ITEMS) break;
                        if(res == ERROR_SUCCESS) {
                            std::wstring key = std::wstring(progid) + L"\\DefaultIcon";
                            wchar_t iconpath[512] = L"";
                            DWORD iconpath_size = sizeof(progid);

                            if(SHGetValueW(HKEY_CLASSES_ROOT, key.c_str(), nullptr, nullptr, iconpath, &iconpath_size) == ERROR_SUCCESS) {                    
                                OutputDebugStringW(iconpath);
                                return SHStrDupW(iconpath, icon);
                            } else {
                                OutputDebugStringW(L"SHGetValueW 3 failed");
                            }
                        }
                        ++index;
                    }
                    RegCloseKey(hOWP);
                }
                RegCloseKey(hExtKey);
            }
        }
    } else {
        OutputDebugStringW(L"SHGetFileInfoW failed");
    }

    return SHStrDupW(L"", icon);
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

ULONG
CompareWithMRUCommand::getIndex() const { 
    return _index; 
}
