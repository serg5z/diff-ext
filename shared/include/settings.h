/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#pragma once

#include <string>
#include <vector>


bool LoadSettings();
bool SaveSettings();

std::vector<std::wstring> getMRU();
void addToMRU(const std::wstring& path);
void clearMRU();

std::wstring getDiffTool();
void setDiffTool(const std::wstring& tool);

size_t getMRUCapacity();
void setMRUCapacity(size_t capacity);
