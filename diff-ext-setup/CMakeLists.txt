cmake_minimum_required(VERSION 3.15)

project(diff-ext-setup LANGUAGES CXX)

add_executable(diff-ext-setup WIN32
    src/main.cpp
    src/diff-ext-setup.rc
    src/resource.h
    src/diff-ext-setup.manifest
)

# Link Windows libraries
target_link_libraries(diff-ext-setup PRIVATE 
    shared
    windowsapp
    shell32
    shlwapi
    comctl32
    version
)
