cmake_minimum_required(VERSION 3.15)

project(diff-ext VERSION 2.0.0 LANGUAGES CXX)

option(DEVELOPMENT_BUILD "Build with development Appx manifest" OFF)

message(STATUS "DEVELOPMENT_BUILD: ${DEVELOPMENT_BUILD}")

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
find_program(MAKEAPPX_EXECUTABLE makeappx.exe)

if(NOT MAKEAPPX_EXECUTABLE)
    message(FATAL_ERROR "makeappx.exe not found in PATH")
endif()

# Determine architecture for MSIX manifest
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^[Aa][Mm][Dd]64|x86_64$")
    set(PACKAGE_ARCH "x64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86|i[3-6]86)$")
    set(PACKAGE_ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(ARM64|aarch64)$")
    set(PACKAGE_ARCH "arm64")
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

message(STATUS "Detected architecture for MSIX: ${PACKAGE_ARCH}")

set(MSIX_NAME "diff-ext-${PACKAGE_ARCH}.msix")
set(MSIX_OUTPUT_DIR "${CMAKE_BINARY_DIR}/msix")
set(MSIX_BUILD_DIR "${CMAKE_BINARY_DIR}/msix_build")

add_custom_target(msix ALL
    COMMENT "Generating MSIX package"
)

if(DEVELOPMENT_BUILD)
    message(STATUS "Using development manifest")
    set(APPX_MANIFEST_TEMPLATE "${CMAKE_SOURCE_DIR}/manifests/AppxManifest.dev.xml.in")
else()
    message(STATUS "Using production manifest")
    set(APPX_MANIFEST_TEMPLATE "${CMAKE_SOURCE_DIR}/manifests/AppxManifest.xml.in")
endif()

message(STATUS "Appx manifest template: ${APPX_MANIFEST_TEMPLATE}")

configure_file(
    ${APPX_MANIFEST_TEMPLATE}
    ${MSIX_BUILD_DIR}/AppxManifest.xml
    @ONLY
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
    COMMAND ${MAKEAPPX_EXECUTABLE} pack /d "${MSIX_BUILD_DIR}" /p "${MSIX_OUTPUT_DIR}/${MSIX_NAME}" /overwrite
    COMMENT "MSIX package created at ${MSIX_OUTPUT_DIR}/${MSIX_NAME}"
)

# Enable source packaging
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_GENERATOR "")  # Avoid creating binary installers
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES
    "/build.*/.*"
    "/_CPack_Packages/.*"
    "/\\\\.git/.*"
    "/\\\\.vscode/.*"
    "/\\\\.idea/.*"
    "/\\\\.vs/.*"
    "/bin/.*"
    "/lib/.*"
    "/msix/.*"
    "/msix_build/.*"
    "~$"
)

set(CPACK_SOURCE_TOPLEVEL_TAG "")  # Prevent adding platform info to the filename

include(CPack)
