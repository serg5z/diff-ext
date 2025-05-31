cmake_minimum_required(VERSION 3.15)

project(diff-ext-setup LANGUAGES C CXX)

add_executable(diff-ext-setup WIN32
    src/main.cpp
    src/diff-ext-setup.rc
    src/resource.h
    src/diff-ext-setup.manifest
)

target_include_directories(diff-ext-setup PRIVATE
    ${dialog-layout_SOURCE_DIR}/include
)

# Link Windows libraries
target_link_libraries(diff-ext-setup PRIVATE 
    shared
    windowsapp
    shell32
    shlwapi
    comctl32
    version
    dialog-layout
)
