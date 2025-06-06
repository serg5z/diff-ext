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

std::wstring get_shortened_display_path(const std::wstring& path);
bool launch_diff_tool(const std::wstring& file1, const std::wstring& file2);