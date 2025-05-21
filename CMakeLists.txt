cmake_minimum_required(VERSION 3.15)

project(diff-ext VERSION 2.0.0 LANGUAGES CXX)

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


# Determine architecture name (e.g., x64, x86, arm64)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_SUFFIX "x64")
else()
    set(ARCH_SUFFIX "x86")
endif()

# Override with Visual Studio platform name if defined
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
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/manifests/AppxManifest-${ARCH_SUFFIX}.xml" "${MSIX_BUILD_DIR}/AppxManifest.xml"
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
