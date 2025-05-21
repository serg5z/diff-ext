cmake_minimum_required(VERSION 3.15)

project(diff-ext LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

add_definitions(-DUNICODE -D_UNICODE)

# Use static runtime (MultiThreaded = /MT, MultiThreadedDebug = /MTd)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subprojects (defines the actual targets)
add_subdirectory(shared)
add_subdirectory(classic)
add_subdirectory(modern)
add_subdirectory(diff-ext-setup)

# MSIX config
find_program(MAKEAPPX_EXE makeappx REQUIRED)
if(NOT MAKEAPPX_EXE)
    message(FATAL_ERROR "makeappx.exe not found. Install Windows SDK.")
endif()

# Determine architecture name (e.g., x64, x86, arm64)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_SUFFIX "x64")
else()
    set(ARCH_SUFFIX "x86")
endif()

# Optional: override with Visual Studio platform name if defined
if(CMAKE_VS_PLATFORM_NAME)
    set(ARCH_SUFFIX "${CMAKE_VS_PLATFORM_NAME}")
endif()

set(MSIX_NAME "diff-ext-${ARCH_SUFFIX}.msix")
set(MSIX_OUTPUT_DIR "${CMAKE_BINARY_DIR}/msix")
set(MSIX_BUILD_DIR "${CMAKE_BINARY_DIR}/msix_build")

add_custom_target(msix ALL
    COMMENT "Generating MSIX package"
)

# Copy files into MSIX staging directory
add_custom_command(
    TARGET msix PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${MSIX_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/packaging" "${MSIX_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:diff-ext-setup>" "${MSIX_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:diff-ext-modern>" "${MSIX_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:diff-ext-classic>" "${MSIX_BUILD_DIR}"
)

# Create the MSIX package
add_custom_command(
    TARGET msix POST_BUILD
    COMMAND ${MAKEAPPX_EXE} pack /d "${MSIX_BUILD_DIR}" /p "${MSIX_OUTPUT_DIR}/${MSIX_NAME}" /overwrite
    COMMENT "MSIX package created at ${MSIX_OUTPUT_DIR}/${MSIX_NAME}"
)
