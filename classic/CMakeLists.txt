cmake_minimum_required(VERSION 3.15)

project(ClassicContextMenu LANGUAGES CXX)

# Create a DLL target for the Classic Context Menu extension
add_library(diff-ext-classic SHARED
    src/contextmenu.h
    src/contextmenu.cpp
    src/dllmain.cpp
    src/diff-ext-classic.def
    src/resources.h
    src/resources.rc
    src/icons/compare.ico
    src/icons/remember.ico
)

# Include directories (if MRUManager exports include dirs, you might skip this)
target_include_directories(diff-ext-classic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set output directories (optional, usually inherited)
set_target_properties(diff-ext-classic PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add any required Windows libraries
target_link_libraries(diff-ext-classic PRIVATE
    shared
    user32
    shell32
    ole32
    uuid
    runtimeobject
    shlwapi
    gdiplus
)
