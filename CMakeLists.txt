cmake_minimum_required(VERSION 4.0)

project(WildFoxEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Module scanning is enabled per target only when .cppm files exist (see below)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# ccache is optional: use it only if it is installed
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

include(FetchContent)

# Default to Release when no build type is given (physics is much slower in Debug)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# -DWFE_PORTABLE=ON  -> binary for other people: relative assets path, bundled libs, cpack archive
# -DWFE_PORTABLE=OFF -> local development build (absolute paths into the build directory)
option(WFE_PORTABLE "Build a relocatable package for distribution" OFF)

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

# ========== Glad =============
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

# ========== Stb_image =============
add_library(stb_image STATIC vendor/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC vendor/stb_image)

# ========== ImGui ==========
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)
set(IMGUI_INCLUDE_DIRS
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIRS})
target_link_libraries(imgui PUBLIC glad)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

# ========== ImGuizmo ==========
set(IMGUIZMO_DIR ${CMAKE_SOURCE_DIR}/vendor/imguizmo)
set(IMGUIZMO_SOURCES
        ${IMGUIZMO_DIR}/ImSequencer.cpp
        ${IMGUIZMO_DIR}/ImGuizmo.cpp
        ${IMGUIZMO_DIR}/ImGradient.cpp
        ${IMGUIZMO_DIR}/ImCurveEdit.cpp
        ${IMGUIZMO_DIR}/GraphEditor.cpp
)
add_library(imguizmo STATIC ${IMGUIZMO_SOURCES})

target_include_directories(imguizmo PUBLIC ${IMGUIZMO_DIR})
target_link_libraries(imguizmo PUBLIC imgui)

# ========== ImGuiColorTextEdit =====
set(IMGUI_TEXTEDIT_DIR ${CMAKE_SOURCE_DIR}/vendor/imguitexteditor)
set(IMGUI_TEXTEDIT_SOURCES
        ${IMGUI_TEXTEDIT_DIR}/TextDiff.cpp
        ${IMGUI_TEXTEDIT_DIR}/TextEditor.cpp
        ${IMGUI_TEXTEDIT_DIR}/extras/LspBridge.cpp
        ${IMGUI_TEXTEDIT_DIR}/extras/TrieAutoComplete.cpp
)
add_library(imguiTextEdit STATIC ${IMGUI_TEXTEDIT_SOURCES})

target_include_directories(imguiTextEdit PUBLIC ${IMGUI_TEXTEDIT_DIR})
target_link_libraries(imguiTextEdit PUBLIC imgui lsp)

# ========== LSP ====================
FetchContent_Declare(
        lsp
        GIT_REPOSITORY https://github.com/leon-bckl/lsp-framework.git
        GIT_TAG master
)

set(LSP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(lsp)

# ========== ImGuiFileSelector / ImGuiToastr ==========
# SOURCE_SUBDIR points nowhere so only sources are fetched, no add_subdirectory
FetchContent_Declare(
        imguifileselector
        GIT_REPOSITORY https://github.com/goossens/ImGuiFileSelector.git
        GIT_TAG v1.93.0
        SOURCE_SUBDIR _none
)
FetchContent_Declare(
        imguitoastr
        GIT_REPOSITORY https://github.com/goossens/ImGuiToastr.git
        GIT_TAG v1.93.0
        SOURCE_SUBDIR _none
)
FetchContent_MakeAvailable(imguifileselector imguitoastr)

add_library(imguiFileSelector STATIC ${imguifileselector_SOURCE_DIR}/FileSelector.cpp)
target_include_directories(imguiFileSelector PUBLIC ${imguifileselector_SOURCE_DIR})
target_link_libraries(imguiFileSelector PUBLIC imgui)
target_compile_definitions(imguiFileSelector PRIVATE IMGUI_DEFINE_MATH_OPERATORS)

add_library(imguiToastr STATIC ${imguitoastr_SOURCE_DIR}/Toastr.cpp)
target_include_directories(imguiToastr PUBLIC ${imguitoastr_SOURCE_DIR})
target_link_libraries(imguiToastr PUBLIC imgui)

# ========== Magic_enum =============
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${CMAKE_SOURCE_DIR}/vendor/magic_enum/include)

# ========== ENTT =============
add_subdirectory(vendor/entt)

# ========== nlohmann_json =============
add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_SOURCE_DIR}/vendor/nlohmann_json)

# ========== AngelScript ==============
add_subdirectory(vendor/angelscript/sdk/angelscript/projects/cmake)

add_library(scriptbuilder STATIC
        vendor/angelscript/sdk/add_on/scriptbuilder/scriptbuilder.cpp)

target_include_directories(scriptbuilder PUBLIC
        vendor/angelscript/sdk/add_on/scriptbuilder
        vendor/angelscript/sdk/angelscript/include
)

add_library(scriptstdstring STATIC
        vendor/angelscript/sdk/add_on/scriptstdstring/scriptstdstring.cpp)

target_include_directories(scriptstdstring PUBLIC
        vendor/angelscript/sdk/add_on/scriptstdstring
        vendor/angelscript/sdk/angelscript/include
)

target_link_libraries(scriptstdstring PUBLIC angelscript)

# ========== WildFoxEngine =============
include_directories(${IMGUI_INCLUDE_DIRS})

file(GLOB_RECURSE WildFoxEngine_ALL_SOURCES CONFIGURE_DEPENDS
        "src/*.cpp"
        "src/*.c"
)

list(FILTER WildFoxEngine_ALL_SOURCES EXCLUDE REGEX ".*/old/.*")

file(GLOB_RECURSE WildFoxEngine_ALL_HEADERS CONFIGURE_DEPENDS
        "src/*.h"
        "src/*.hpp"
)
list(FILTER WildFoxEngine_ALL_HEADERS EXCLUDE REGEX ".*/old/.*")

file(GLOB_RECURSE WildFoxEngine_MODULES CONFIGURE_DEPENDS
        "src/*.cppm"
)
list(FILTER WildFoxEngine_MODULES EXCLUDE REGEX ".*/old/.*")

set(WildFoxEngine_SOURCES ${WildFoxEngine_ALL_SOURCES})
if(WildFoxEngine_MODULES)
    list(FILTER WildFoxEngine_SOURCES EXCLUDE REGEX ".*\\.cppm$")
endif()

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/src/main.cpp)
    message(FATAL_ERROR "src/main.cpp not found!")
endif()

message(STATUS "=== Found files ===")
message(STATUS "C++ files: ${WildFoxEngine_SOURCES}")
message(STATUS "Headers: ${WildFoxEngine_ALL_HEADERS}")

add_executable(${PROJECT_NAME}
        ${WildFoxEngine_SOURCES}
        ${WildFoxEngine_ALL_HEADERS}
)

# set_target_properties(${PROJECT_NAME} PROPERTIES
# UNITY_BUILD ON
# UNITY_BUILD_BATCH_SIZE 16
# )

foreach(MODULE ${WildFoxEngine_MODULES})
    set_source_files_properties(${MODULE} PROPERTIES
            SKIP_UNITY_BUILD_INCLUSION ON
            SKIP_PRECOMPILE_HEADERS ON
    )
endforeach()

if(WildFoxEngine_MODULES)
    message(STATUS "=== C++ Modules ===")
    foreach(MODULE ${WildFoxEngine_MODULES})
        message(STATUS "  - ${MODULE}")
    endforeach()

    target_sources(${PROJECT_NAME}
            PUBLIC FILE_SET CXX_MODULES FILES
            ${WildFoxEngine_MODULES}
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_SCAN_FOR_MODULES ON
            CXX_MODULE_STD ""
            CCACHE_SLOPPINESS "pch_defines,time_macros"
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
        # OpenGL::GL
        glad
        glfw
        glm::glm
        stb_image
        assimp
        imgui
        imguizmo
        imguiTextEdit
        imguiFileSelector
        imguiToastr
        magic_enum
        EnTT::EnTT
        angelscript
        scriptbuilder
        scriptstdstring
        OpenAL::OpenAL
        sndfile
        json
        lsp
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
        <glad/glad.h>
        [["glm_fix.h"]]
        <glm/glm.hpp>
        <glm/gtc/quaternion.hpp>
        <entt/entt.hpp>
        <nlohmann/json.hpp>
        <string>
        <vector>
        <unordered_map>
        <memory>
)

# mold (if installed). target_link_options is used so that it really applies to this target.
find_program(MOLD_LINKER mold)
if(MOLD_LINKER)
    target_link_options(${PROJECT_NAME} PRIVATE -fuse-ld=mold)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE
            -O0
            -g
            -fno-omit-frame-pointer
    )
endif()
# Release uses CMake's default flags (-O3 -DNDEBUG)

# Assets next to the binary in the build directory (dev runs, and the source of the package)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
        COMMENT "Copying assets to build directory"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# ========== Paths baked into the binary ==========
if(WFE_PORTABLE)
    # Relative to the working directory. main() must chdir to the executable's folder:
    #   std::filesystem::current_path(std::filesystem::canonical("/proc/self/exe").parent_path());
    set(WFE_ASSETS_PATH "assets")
    set(WFE_PROJECT_ROOT ".")
else()
    set(WFE_ASSETS_PATH "${CMAKE_BINARY_DIR}/assets")
    set(WFE_PROJECT_ROOT "${CMAKE_SOURCE_DIR}")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        ASSETS_PATH="${WFE_ASSETS_PATH}"
        PROJECT_ROOT="${WFE_PROJECT_ROOT}"
        GLM_ENABLE_EXPERIMENTAL
)

# ========== Install / package ==========
if(WFE_PORTABLE)
    # Do not depend on the libstdc++ of the tester's system
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(${PROJECT_NAME} PRIVATE -static-libstdc++ -static-libgcc)
    endif()

    # Libraries are looked up in ./lib next to the executable
    set_target_properties(${PROJECT_NAME} PROPERTIES
            INSTALL_RPATH "$ORIGIN/lib"
    )

    install(TARGETS ${PROJECT_NAME}
            RUNTIME_DEPENDENCY_SET wfe_runtime_deps
            RUNTIME DESTINATION .
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION .)

    # Bundle non-system shared libraries (glfw, OpenAL, assimp, sndfile and their dependencies).
    # System libraries (libc, GPU drivers, X11/Wayland) must come from the tester's machine.
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
else()
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()