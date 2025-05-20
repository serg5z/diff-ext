#pragma once

#include <string>

std::wstring get_shortened_display_path(const std::wstring& path);
bool launch_diff_tool(const std::wstring& file1, const std::wstring& file2);