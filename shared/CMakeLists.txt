cmake_minimum_required(VERSION 3.15)

project(shared LANGUAGES CXX)

add_library(shared STATIC
    include/settings.h
    src/settings.cpp
    include/util.h
    src/util.cpp
    include/classfactory.h
)

target_include_directories(shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# If MRUManager uses Windows API:
target_link_libraries(shared PRIVATE
    user32
    shell32
    nlohmann_json::nlohmann_json
)

target_compile_definitions(shared PRIVATE UNICODE _UNICODE)

# Optional: set output directory
set_target_properties(shared PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

