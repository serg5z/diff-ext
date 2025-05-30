/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include <windows.h>
#include <shlobj.h>

#include <fstream>

#include <nlohmann/json.hpp>

#include "settings.h"


size_t _MRUCapacity = 16;
std::wstring _diffTool = L"";
std::vector<std::wstring> _MRU;

static std::wstring 
get_mru_file_path() {
    PWSTR app_data_path = nullptr;
    std::wstring path = L"settings.json";

    if(SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &app_data_path))) {
        path = app_data_path;
        CoTaskMemFree(app_data_path);
        path += L"\\diff-ext\\settings.json";
    }

    return path;
}

static std::string 
WStringToUtf8(const std::wstring& wstr) {
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string
    utf8(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8[0], len, nullptr, nullptr);
    utf8.resize(strlen(utf8.c_str())); // Remove null terminator padding
    return utf8;
}

static std::wstring 
Utf8ToWString(const std::string& utf8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring
    wstr(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], len);
    wstr.resize(wcslen(wstr.c_str())); // Remove null terminator padding
    return wstr;
}

bool
LoadSettings() {
    std::ifstream
    file(get_mru_file_path());

    if(!file.is_open()) {
        return false;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch(...) {
        return false;
    }

    if(j.contains("diff_tool")) {
        _diffTool = Utf8ToWString(j["diff_tool"].get<std::string>());
    }

    if(j.contains("mru_capacity")) {
        _MRUCapacity = j["mru_capacity"].get<int>();
    }

    if(j.contains("mru_list") && j["mru_list"].is_array()) {
        _MRU.clear();
        for(const auto& item : j["mru_list"]) {
            _MRU.push_back(Utf8ToWString(item.get<std::string>()));
        }
        
        if(_MRU.size() > _MRUCapacity) {
            _MRU.resize(_MRUCapacity);
        }
    }

    return true;
}

bool
SaveSettings() {
    std::wstring dir = get_mru_file_path();

    CreateDirectoryW(dir.substr(0, dir.find_last_of(L'\\')).c_str(), nullptr);

    nlohmann::json j;
    
    j["diff_tool"] = WStringToUtf8(_diffTool);
    j["mru_capacity"] = _MRUCapacity;
    j["mru_list"] = nlohmann::
    json::array();
    
    for(const auto& item : _MRU) {
        j["mru_list"].push_back(WStringToUtf8(item));
    }

    std::ofstream
    file(get_mru_file_path(), std::ios::trunc);

    if(!file.is_open()) {
        return false;
    }

    try {
        file << j.dump(4);
    } catch(...) {
        return false;
    }

    return true;
}

std::vector<std::wstring>
getMRU() {
    return _MRU;
}

void
addToMRU(const std::wstring& path) {
    // Remove if already in list
    auto it = std::find(_MRU.begin(), _MRU.end(), path);
    if(it != _MRU.end())
        _MRU.erase(it);

    // Insert at top
    _MRU.insert(_MRU.begin(), path);

    // Trim
    if(_MRU.size() > _MRUCapacity) {
        _MRU.resize(_MRUCapacity);
    }
}

void
clearMRU() {
    _MRU.clear();
}

std::wstring
getDiffTool() {
    return _diffTool;
}

void
setDiffTool(const std::wstring& tool) {
    _diffTool = tool;
}

size_t
getMRUCapacity() {
    return _MRUCapacity;
}

void
setMRUCapacity(size_t capacity) {
    if(capacity >= 1 && capacity <= 64) {
        _MRUCapacity = capacity;
    }
}
