cmake_minimum_required(VERSION 3.28)

project(WildFoxEngine LANGUAGES C CXX)

# ==============================================================================
# Quick start
#
#   Linux:
#     cmake -S . -B build -G Ninja
#     cmake --build build
#
#   Windows (Visual Studio 2022, from a normal terminal):
#     cmake -S . -B build -G "Visual Studio 17 2022"
#     cmake --build build --config Release
#
#   Windows (Ninja, from "x64 Native Tools Command Prompt"):
#     cmake -S . -B build -G Ninja
#
#   Optional: -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
#   Every system dependency below is first searched with find_package() (so
#   vcpkg / pacman / apt packages are used when present) and downloaded and
#   built from source only as a fallback.
#
#   Requirements: Git, Python 3 (glad1 generates its loader at configure time).
# ==============================================================================

# ------------------------------------------------------------------------------
# Language / compiler settings
# ------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# ccache speeds up rebuilds on GCC/Clang. MSVC does not work well with it.
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM AND NOT MSVC)
    set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ------------------------------------------------------------------------------
# User options
# ------------------------------------------------------------------------------
option(WFE_PORTABLE "Build a relocatable package for distribution" OFF)

option(WFE_FETCH_ALL_DEPS
        "Ignore system packages and always build dependencies from source" OFF)

# How the 'assets' folder becomes visible next to the build output:
#   LINK   - symlink (junction on Windows) to the source folder. Nothing is
#            copied, edits in the repo are visible instantly. (default)
#   COPY   - incremental copy: only changed files are copied, and only when the
#            'wfe_assets' target runs (it never forces a relink of the exe).
#   SOURCE - no build-dir copy at all, ASSETS_PATH points to the source tree.
set(WFE_ASSETS_MODE "LINK" CACHE STRING "How assets are provided to the build directory")
set_property(CACHE WFE_ASSETS_MODE PROPERTY STRINGS LINK COPY SOURCE)

# ------------------------------------------------------------------------------
# Dependency handling
# ------------------------------------------------------------------------------
include(FetchContent)

# Do not hit the network on every re-configure. Delete build/_deps/<name>-subbuild
# (or the whole build dir) to force an update. Pin commits instead of 'master'
# if you need fully reproducible builds.
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

if(WFE_FETCH_ALL_DEPS)
    set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
endif()

# CMake 4 rejects third-party projects with cmake_minimum_required < 3.5
# (AngelScript, glad1 and others still declare old minimums).
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Build fetched libraries statically where possible: fewer DLLs on Windows.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Helper: make sure <ALIAS_NAME> exists, aliasing the first existing candidate.
# Keeps the engine's target names identical whether a library came from the
# system or from FetchContent.
function(wfe_ensure_target ALIAS_NAME)
    if(TARGET ${ALIAS_NAME})
        return()
    endif()
    foreach(candidate IN LISTS ARGN)
        if(TARGET ${candidate})
            add_library(${ALIAS_NAME} ALIAS ${candidate})
            return()
        endif()
    endforeach()
    message(FATAL_ERROR "Could not find a target for ${ALIAS_NAME} (tried: ${ARGN})")
endfunction()

# ---------- Always from the system (available on all platforms) ----------
find_package(OpenGL REQUIRED)

# ---------- Options for libraries that may be built from source ----------
# GLFW
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# assimp
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

# nlohmann_json
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install    OFF CACHE BOOL "" FORCE)

# OpenAL Soft
set(ALSOFT_UTILS        OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES     OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS        OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL      OFF CACHE BOOL "" FORCE)
set(ALSOFT_NO_CONFIG_UTIL ON CACHE BOOL "" FORCE)

# libsndfile (without external codecs the fallback build reads/writes WAV/AIFF
# and similar. For OGG/FLAC/Opus use vcpkg or system packages instead.)
set(BUILD_TESTING         OFF CACHE BOOL "" FORCE)
set(BUILD_PROGRAMS        OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES        OFF CACHE BOOL "" FORCE)
set(ENABLE_EXTERNAL_LIBS  OFF CACHE BOOL "" FORCE)
set(ENABLE_MPEG           OFF CACHE BOOL "" FORCE)
set(ENABLE_CPACK          OFF CACHE BOOL "" FORCE)
set(ENABLE_PACKAGE_CONFIG OFF CACHE BOOL "" FORCE)
set(INSTALL_PKGCONFIG_MODULE OFF CACHE BOOL "" FORCE)
set(INSTALL_MANPAGES      OFF CACHE BOOL "" FORCE)

# magic_enum
set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MAGIC_ENUM_OPT_BUILD_TESTS    OFF CACHE BOOL "" FORCE)

# glad1: OpenGL 4.6 core profile, C loader
set(GLAD_PROFILE   "core"   CACHE STRING "" FORCE)
set(GLAD_API       "gl=4.6" CACHE STRING "" FORCE)
set(GLAD_GENERATOR "c"      CACHE STRING "" FORCE)

# lsp-framework
set(LSP_INSTALL OFF CACHE BOOL "" FORCE)

# ---------- Declarations ----------
# Libraries with FIND_PACKAGE_ARGS: the system package is used when found,
# otherwise the source below is downloaded and built.
FetchContent_Declare(glfw3
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG        3.4
        GIT_SHALLOW    TRUE
        FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG        1.0.1
        GIT_SHALLOW    TRUE
        FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(assimp
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG        v5.4.3
        GIT_SHALLOW    TRUE
        FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG        v3.11.3
        GIT_SHALLOW    TRUE
        FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(OpenAL
        GIT_REPOSITORY https://github.com/kcat/openal-soft
        GIT_TAG        1.23.1
        GIT_SHALLOW    TRUE
        FIND_PACKAGE_ARGS
)
FetchContent_Declare(SndFile
        GIT_REPOSITORY https://github.com/libsndfile/libsndfile
        GIT_TAG        1.2.2
        GIT_SHALLOW    TRUE
        FIND_PACKAGE_ARGS CONFIG
)

# Libraries that are always fetched.
FetchContent_Declare(glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG        v0.1.36
        GIT_SHALLOW    TRUE
)
FetchContent_Declare(EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt
        GIT_TAG        v3.16.0
        GIT_SHALLOW    TRUE
)
FetchContent_Declare(magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum
        GIT_TAG        v0.9.7
        GIT_SHALLOW    TRUE
)
# stb is header-only: SOURCE_SUBDIR _none = download only, no add_subdirectory.
FetchContent_Declare(stb
        GIT_REPOSITORY https://github.com/nothings/stb
        GIT_TAG        master            # pin a commit for reproducible builds
        GIT_SHALLOW    TRUE
        SOURCE_SUBDIR  _none
)
FetchContent_Declare(angelscript
        GIT_REPOSITORY https://github.com/anjo76/angelscript
        GIT_TAG        master            # pin a commit for reproducible builds
        GIT_SHALLOW    TRUE
        SOURCE_SUBDIR  sdk/angelscript/projects/cmake
)
# ImGui and its extensions have no (usable) CMake projects: we build them below.
FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG        v1.93.0-docking   # must match the FileSelector/Toastr tags; the editor needs docking + viewports
        GIT_SHALLOW    TRUE
        SOURCE_SUBDIR  _none
)
FetchContent_Declare(imguizmo
        GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo
        GIT_TAG        master            # pin a commit compatible with your imgui version
        GIT_SHALLOW    TRUE
        SOURCE_SUBDIR  _none
)
FetchContent_Declare(imguifileselector
        GIT_REPOSITORY https://github.com/goossens/ImGuiFileSelector
        GIT_TAG        v1.93.0
        GIT_SHALLOW    TRUE
        SOURCE_SUBDIR  _none
)
FetchContent_Declare(imguitoastr
        GIT_REPOSITORY https://github.com/goossens/ImGuiToastr
        GIT_TAG        v1.93.0
        GIT_SHALLOW    TRUE
        SOURCE_SUBDIR  _none
)
FetchContent_Declare(imguitexteditor
        GIT_REPOSITORY https://github.com/goossens/ImGuiColorTextEdit
        GIT_TAG        master
        GIT_SHALLOW    TRUE
        SOURCE_SUBDIR  _none
)
FetchContent_Declare(lsp
        GIT_REPOSITORY https://github.com/leon-bckl/lsp-framework
        GIT_TAG        master
        GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(
        glfw3 glm assimp nlohmann_json OpenAL SndFile
        glad EnTT magic_enum stb angelscript
        imgui imguizmo imguifileselector imguitoastr imguitexteditor
        lsp
)

# ---------- Normalize target names (system vs. fetched) ----------
wfe_ensure_target(glm::glm                    glm)
wfe_ensure_target(assimp::assimp              assimp)
wfe_ensure_target(nlohmann_json::nlohmann_json nlohmann_json)
wfe_ensure_target(OpenAL::OpenAL              OpenAL openal)
wfe_ensure_target(SndFile::sndfile            sndfile)
wfe_ensure_target(glad::glad                  glad)
wfe_ensure_target(Angelscript::angelscript    Angelscript angelscript)

# ---------- lsp-framework: MSVC patch ----------
# Applied only when needed so the file is not rewritten (and rebuilt) on every
# configure.
if(MSVC)
    set(LSP_ENDPOINT_FILE "${lsp_SOURCE_DIR}/lsp/endpoint_base.h")
    file(READ "${LSP_ENDPOINT_FILE}" LSP_ENDPOINT_OLD)
    string(REPLACE "static_assert(false,"
            "static_assert(!std::is_same_v<F, F>,"
            LSP_ENDPOINT_NEW "${LSP_ENDPOINT_OLD}")
    if(NOT LSP_ENDPOINT_NEW STREQUAL LSP_ENDPOINT_OLD)
        file(WRITE "${LSP_ENDPOINT_FILE}" "${LSP_ENDPOINT_NEW}")
    endif()
endif()

# ---------- AngelScript add-ons (not part of the library itself) ----------
set(AS_ADDON_DIR "${angelscript_SOURCE_DIR}/sdk/add_on")

add_library(scriptbuilder STATIC "${AS_ADDON_DIR}/scriptbuilder/scriptbuilder.cpp")
target_include_directories(scriptbuilder PUBLIC "${AS_ADDON_DIR}/scriptbuilder")
target_link_libraries(scriptbuilder PUBLIC Angelscript::angelscript)

add_library(scriptstdstring STATIC "${AS_ADDON_DIR}/scriptstdstring/scriptstdstring.cpp")
target_include_directories(scriptstdstring PUBLIC "${AS_ADDON_DIR}/scriptstdstring")
target_link_libraries(scriptstdstring PUBLIC Angelscript::angelscript)

# ---------- stb_image (implementation compiled once) ----------
file(WRITE "${CMAKE_BINARY_DIR}/stb_image_impl.cpp"
        "#define STB_IMAGE_IMPLEMENTATION\n#include <stb_image.h>\n")
add_library(stb_image STATIC "${CMAKE_BINARY_DIR}/stb_image_impl.cpp")
target_include_directories(stb_image PUBLIC "${stb_SOURCE_DIR}")

# ---------- ImGui (core + GLFW/OpenGL3 backends + std::string helpers) ----------
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${imgui_SOURCE_DIR}/misc/cpp
)
target_link_libraries(imgui PUBLIC glfw)
add_library(imgui::imgui ALIAS imgui)

# ---------- ImGuizmo ----------
# Upstream moved the sources into src/; support both layouts.
if(EXISTS ${imguizmo_SOURCE_DIR}/src/ImGuizmo.cpp)
    set(IMGUIZMO_DIR ${imguizmo_SOURCE_DIR}/src)
else()
    set(IMGUIZMO_DIR ${imguizmo_SOURCE_DIR})
endif()
add_library(imguizmo STATIC ${IMGUIZMO_DIR}/ImGuizmo.cpp)
target_include_directories(imguizmo PUBLIC ${IMGUIZMO_DIR})
target_link_libraries(imguizmo PUBLIC imgui)
add_library(imguizmo::imguizmo ALIAS imguizmo)

# ---------- ImGuiFileSelector ----------
add_library(imguiFileSelector STATIC ${imguifileselector_SOURCE_DIR}/FileSelector.cpp)
target_include_directories(imguiFileSelector PUBLIC ${imguifileselector_SOURCE_DIR})
target_link_libraries(imguiFileSelector PUBLIC imgui::imgui)
target_compile_definitions(imguiFileSelector PRIVATE IMGUI_DEFINE_MATH_OPERATORS)

# ---------- ImGuiToastr ----------
add_library(imguiToastr STATIC ${imguitoastr_SOURCE_DIR}/Toastr.cpp)
target_include_directories(imguiToastr PUBLIC ${imguitoastr_SOURCE_DIR})
target_link_libraries(imguiToastr PUBLIC imgui::imgui)

# ---------- ImGuiColorTextEdit (+ LSP bridge) ----------
add_library(imguiTextEdit STATIC
        ${imguitexteditor_SOURCE_DIR}/TextDiff.cpp
        ${imguitexteditor_SOURCE_DIR}/TextEditor.cpp
        ${imguitexteditor_SOURCE_DIR}/extras/LspBridge.cpp
        ${imguitexteditor_SOURCE_DIR}/extras/TrieAutoComplete.cpp
)
target_include_directories(imguiTextEdit PUBLIC ${imguitexteditor_SOURCE_DIR})
target_link_libraries(imguiTextEdit PUBLIC imgui::imgui lsp)

# ==============================================================================
# WildFoxEngine
# ==============================================================================
file(GLOB_RECURSE WildFoxEngine_ALL_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.c")
list(FILTER WildFoxEngine_ALL_SOURCES EXCLUDE REGEX ".*/old/.*")

file(GLOB_RECURSE WildFoxEngine_ALL_HEADERS CONFIGURE_DEPENDS "src/*.h" "src/*.hpp")
list(FILTER WildFoxEngine_ALL_HEADERS EXCLUDE REGEX ".*/old/.*")

file(GLOB_RECURSE WildFoxEngine_MODULES CONFIGURE_DEPENDS "src/*.cppm")
list(FILTER WildFoxEngine_MODULES EXCLUDE REGEX ".*/old/.*")

set(WildFoxEngine_SOURCES ${WildFoxEngine_ALL_SOURCES})
if(WildFoxEngine_MODULES)
    list(FILTER WildFoxEngine_SOURCES EXCLUDE REGEX ".*\\.cppm$")
endif()

add_executable(${PROJECT_NAME}
        ${WildFoxEngine_SOURCES}
        ${WildFoxEngine_ALL_HEADERS}
)

foreach(MODULE ${WildFoxEngine_MODULES})
    set_source_files_properties(${MODULE} PROPERTIES
            SKIP_UNITY_BUILD_INCLUSION ON
            SKIP_PRECOMPILE_HEADERS ON
    )
endforeach()

if(WildFoxEngine_MODULES)
    target_sources(${PROJECT_NAME}
            PUBLIC FILE_SET CXX_MODULES FILES
            ${WildFoxEngine_MODULES}
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_SCAN_FOR_MODULES ON
            CXX_MODULE_STD ""
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL
        glad::glad
        glfw
        glm::glm
        stb_image
        assimp::assimp
        imgui::imgui
        imguizmo::imguizmo
        imguiTextEdit
        imguiFileSelector
        imguiToastr
        magic_enum::magic_enum
        EnTT::EnTT
        Angelscript::angelscript
        scriptbuilder
        scriptstdstring
        OpenAL::OpenAL
        SndFile::sndfile
        nlohmann_json::nlohmann_json
        lsp
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
        <string>
        <vector>
        <unordered_map>
        <memory>
        <entt/entt.hpp>
        <nlohmann/json.hpp>
        <glad/glad.h>
        <glm/glm.hpp>
        <glm/gtc/quaternion.hpp>
)

# ---------- Compiler / platform specific settings ----------
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /utf-8              # source and execution charset = UTF-8
            /MP                 # parallel compilation inside one target
            /Zc:__cplusplus     # report the real __cplusplus value
    )
else()
    # Generator expression so it also works with multi-config generators.
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:-O0 -g -fno-omit-frame-pointer>
    )
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            NOMINMAX
            WIN32_LEAN_AND_MEAN
            _CRT_SECURE_NO_WARNINGS
    )

    # Copy the DLLs the exe depends on (OpenAL, ...) next to it. Static
    # libraries produce no DLLs; in that case the command is a no-op.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E
            $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>>,copy_if_different,true>
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND_EXPAND_LISTS
            COMMENT "Copying runtime DLLs"
    )
endif()

# ---------- Assets ----------
# The old setup ran copy_directory after EVERY build (full copy + relink-like
# cost). Now the assets are linked once at configure time, or copied
# incrementally by a separate target.
set(WFE_ASSETS_SRC "${CMAKE_SOURCE_DIR}/assets")
set(WFE_ASSETS_DST "${CMAKE_BINARY_DIR}/assets")
set(WFE_ASSETS_NEED_COPY FALSE)

if(EXISTS "${WFE_ASSETS_SRC}")
    if(WFE_ASSETS_MODE STREQUAL "LINK")
        if(IS_SYMLINK "${WFE_ASSETS_DST}")
            # Already linked from a previous configure.
        elseif(EXISTS "${WFE_ASSETS_DST}")
            # A real directory (e.g. from an older copy-based build) is in the
            # way: do not delete it, fall back to the incremental copy.
            message(STATUS "build/assets already exists and is not a link; using incremental copy. "
                    "Delete it and re-configure to switch to LINK mode.")
            set(WFE_ASSETS_NEED_COPY TRUE)
        else()
            file(CREATE_LINK "${WFE_ASSETS_SRC}" "${WFE_ASSETS_DST}"
                    RESULT WFE_LINK_RESULT SYMBOLIC)
            if(NOT WFE_LINK_RESULT STREQUAL "0" AND WIN32)
                # Symlinks need Developer Mode/admin on Windows; a directory
                # junction does not.
                file(TO_NATIVE_PATH "${WFE_ASSETS_DST}" WFE_NATIVE_DST)
                file(TO_NATIVE_PATH "${WFE_ASSETS_SRC}" WFE_NATIVE_SRC)
                execute_process(
                        COMMAND cmd /c mklink /J "${WFE_NATIVE_DST}" "${WFE_NATIVE_SRC}"
                        RESULT_VARIABLE WFE_LINK_RESULT
                        OUTPUT_QUIET ERROR_QUIET
                )
            endif()
            if(NOT WFE_LINK_RESULT STREQUAL "0")
                message(STATUS "Could not link assets, using incremental copy instead")
                set(WFE_ASSETS_NEED_COPY TRUE)
            endif()
        endif()
    elseif(WFE_ASSETS_MODE STREQUAL "COPY")
        set(WFE_ASSETS_NEED_COPY TRUE)
    endif()
endif()

if(WFE_ASSETS_NEED_COPY)
    # copy_directory_if_different (CMake 3.26+) only touches changed files.
    add_custom_target(wfe_assets ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            "${WFE_ASSETS_SRC}" "${WFE_ASSETS_DST}"
            COMMENT "Syncing assets"
            VERBATIM
    )
    add_dependencies(${PROJECT_NAME} wfe_assets)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# ---------- Paths baked into the binary ----------
if(WFE_PORTABLE)
    set(WFE_ASSETS_PATH  "assets")
    set(WFE_PROJECT_ROOT ".")
else()
    if(WFE_ASSETS_MODE STREQUAL "SOURCE")
        set(WFE_ASSETS_PATH "${WFE_ASSETS_SRC}")
    else()
        set(WFE_ASSETS_PATH "${WFE_ASSETS_DST}")
    endif()
    set(WFE_PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        ASSETS_PATH="${WFE_ASSETS_PATH}"
        PROJECT_ROOT="${WFE_PROJECT_ROOT}"
        GLM_ENABLE_EXPERIMENTAL
        GLM_FORCE_XYZW_ONLY
)

# ==============================================================================
# Install / package: Linux portable release
# ==============================================================================
if(WFE_PORTABLE AND UNIX AND NOT APPLE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(${PROJECT_NAME} PRIVATE -static-libstdc++ -static-libgcc)
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
            INSTALL_RPATH "$ORIGIN/lib"
    )

    install(TARGETS ${PROJECT_NAME}
            RUNTIME_DEPENDENCY_SET wfe_runtime_deps
            RUNTIME DESTINATION .
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION .)

    install(RUNTIME_DEPENDENCY_SET wfe_runtime_deps
            DESTINATION lib
            PRE_EXCLUDE_REGEXES
            [=[ld-linux]=] [=[libc\.so]=] [=[libm\.so]=] [=[libdl\.so]=]
            [=[libpthread\.so]=] [=[librt\.so]=] [=[libresolv\.so]=]
            [=[libgcc_s\.so]=] [=[libstdc\+\+\.so]=]
            [=[libGL]=] [=[libEGL]=] [=[libOpenGL]=] [=[libGLdispatch]=]
            [=[libX]=] [=[libxcb]=] [=[libwayland]=] [=[libdrm]=] [=[libvulkan]=]
            [=[libxkbcommon]=] [=[libdbus]=] [=[libsystemd]=]
    )

    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_NAME "WildFoxEngine")
    set(CPACK_PACKAGE_FILE_NAME "WildFoxEngine-linux")
    include(CPack)
endif()

# ==============================================================================
# Install / package: Windows portable release
# ==============================================================================
if(WFE_PORTABLE AND WIN32)
    if(MSVC)
        # Bundle the MSVC / UCRT runtime so the zip runs on a clean machine.
        set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
        include(InstallRequiredSystemLibraries)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # MinGW: avoid shipping libstdc++/libgcc/winpthread DLLs.
        target_link_options(${PROJECT_NAME} PRIVATE -static)
    endif()

    install(TARGETS ${PROJECT_NAME}
            RUNTIME_DEPENDENCY_SET wfe_runtime_deps
            RUNTIME DESTINATION .
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION .)

    install(RUNTIME_DEPENDENCY_SET wfe_runtime_deps
            DESTINATION .
            PRE_EXCLUDE_REGEXES
            [=[^api-ms-]=] [=[^ext-ms-]=]
            POST_EXCLUDE_REGEXES
            [=[.*[/\\][Ss]ystem32[/\\].*]=]
            [=[.*[/\\][Ww]indows[/\\].*]=]
    )

    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_NAME "WildFoxEngine")
    set(CPACK_PACKAGE_FILE_NAME "WildFoxEngine-windows")
    include(CPack)
endif()