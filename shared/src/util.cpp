#include <windows.h>
#include <shlwapi.h>

#include <string>

#include "settings.h"
#include "util.h"


std::wstring 
get_shortened_display_path(const std::wstring& path) {
    wchar_t buffer[MAX_PATH];

    if(PathCompactPathExW(buffer, path.c_str(), 40, 0)) {
        return buffer;
    }

    return path;
}

bool 
launch_diff_tool(const std::wstring& file1, const std::wstring& file2) {
    // Construct command line: "toolPath" "file1" "file2"
    std::wstring cmdLine = L"\"" + getDiffTool() + L"\" \"" + file1 + L"\" \"" + file2 + L"\"";

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};

    // CreateProcessW needs the command line to be a writable buffer
    std::vector<wchar_t> buffer(cmdLine.begin(), cmdLine.end());
    buffer.push_back(L'\0');

    BOOL success = CreateProcessW(
        nullptr,                 // Application name (null means use from command line)
        buffer.data(),           // Command line
        nullptr,                 // Process security attributes
        nullptr,                 // Thread security attributes
        FALSE,                   // Inherit handles
        0,                       // Creation flags
        nullptr,                 // Environment
        nullptr,                 // Current directory
        &si,
        &pi
    );

    if (success) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess); // Close immediately, or use WaitForSingleObject to wait
        return true;
    } else {
        DWORD err = GetLastError();
        wchar_t msg[256];
        swprintf_s(msg, L"CreateProcess failed with error %lu", err);
        MessageBoxW(nullptr, msg, L"Error launching diff tool", MB_OK | MB_ICONERROR);
        return false;
    }
}
