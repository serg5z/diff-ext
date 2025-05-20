cmake_minimum_required(VERSION 3.15)

project(ModernContextMenu LANGUAGES CXX)

# Create a DLL target for the Modern Context Menu extension
add_library(diff-ext-modern SHARED
    src/basecommand.h
    src/basecommand.cpp
    src/clearmrucommand.h
    src/clearmrucommand.cpp
    src/comparefilescommand.h
    src/comparefilescommand.cpp
    src/comparewithmrucommand.h
    src/comparewithmrucommand.cpp
    src/comparewithtopcommand.h
    src/comparewithtopcommand.cpp
    src/mruenumerator.h
    src/mruenumerator.cpp
    src/rememberfilescommand.h
    src/rememberfilescommand.cpp
    src/mrusubmenu.h
    src/mrusubmenu.cpp
    src/dllmain.cpp
    src/diff-ext-modern.def
)

# Link to MRUManager library
target_link_libraries(diff-ext-modern PRIVATE shared)

# Include directories if needed (MRUManager may export these)
target_include_directories(diff-ext-modern PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Output directories (optional)
set_target_properties(diff-ext-modern PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Link required Windows libs
target_link_libraries(diff-ext-modern PRIVATE
    user32
    shell32
    ole32
    uuid
    shlwapi
    shared
)
