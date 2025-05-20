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
