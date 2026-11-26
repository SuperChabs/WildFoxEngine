cmake_minimum_required(VERSION 3.28)

project(WildFoxEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

include(FetchContent)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(WFE_PORTABLE "Build a relocatable package for distribution" OFF)

# ========== Пакеты из системы ==========
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)
# find_package(EnTT CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(Angelscript CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb_image.h" REQUIRED)

# ========== Stb_image (реализация) ==========

file(WRITE ${CMAKE_BINARY_DIR}/stb_image_impl.cpp "#define STB_IMAGE_IMPLEMENTATION\n#include <stb_image.h>\n")
add_library(stb_image STATIC ${CMAKE_BINARY_DIR}/stb_image_impl.cpp)
target_include_directories(stb_image PUBLIC ${STB_INCLUDE_DIRS})

# ========== FetchContent ==========

FetchContent_Declare(
        angelscript_sdk
        GIT_REPOSITORY https://github.com/anjo76/angelscript
        GIT_TAG master
        SOURCE_SUBDIR _none
)
FetchContent_MakeAvailable(angelscript_sdk)

set(AS_ADDON_DIR "${angelscript_sdk_SOURCE_DIR}/sdk/add_on")

add_library(scriptbuilder STATIC "${AS_ADDON_DIR}/scriptbuilder/scriptbuilder.cpp")
target_include_directories(scriptbuilder PUBLIC "${AS_ADDON_DIR}/scriptbuilder")
target_link_libraries(scriptbuilder PUBLIC Angelscript::angelscript)

add_library(scriptstdstring STATIC "${AS_ADDON_DIR}/scriptstdstring/scriptstdstring.cpp")
target_include_directories(scriptstdstring PUBLIC "${AS_ADDON_DIR}/scriptstdstring")
target_link_libraries(scriptstdstring PUBLIC Angelscript::angelscript)

# ========== FetchContent ==========

set(LSP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        lsp
        GIT_REPOSITORY https://github.com/leon-bckl/lsp-framework
        GIT_TAG master
)
FetchContent_MakeAvailable(lsp)

if(MSVC) # fck u, microslop!
    file(READ "${lsp_SOURCE_DIR}/lsp/endpoint_base.h" LSP_ENDPOINT_H)
    string(REPLACE "static_assert(false," "static_assert(!std::is_same_v<F, F>," LSP_ENDPOINT_H "${LSP_ENDPOINT_H}")
    file(WRITE "${lsp_SOURCE_DIR}/lsp/endpoint_base.h" "${LSP_ENDPOINT_H}")
endif()

# ========== FetchContent ==========

FetchContent_Declare(
        imguifileselector
        GIT_REPOSITORY https://github.com/goossens/ImGuiFileSelector
        GIT_TAG v1.93.0
        SOURCE_SUBDIR _none
)
FetchContent_MakeAvailable(imguifileselector)

add_library(imguiFileSelector STATIC ${imguifileselector_SOURCE_DIR}/FileSelector.cpp)
target_include_directories(imguiFileSelector PUBLIC ${imguifileselector_SOURCE_DIR})
target_link_libraries(imguiFileSelector PUBLIC imgui::imgui)
target_compile_definitions(imguiFileSelector PRIVATE IMGUI_DEFINE_MATH_OPERATORS)

# ========== FetchContent ==========

FetchContent_Declare(
        imguitoastr
        GIT_REPOSITORY https://github.com/goossens/ImGuiToastr
        GIT_TAG v1.93.0
        SOURCE_SUBDIR _none
)
FetchContent_MakeAvailable(imguitoastr)

add_library(imguiToastr STATIC ${imguitoastr_SOURCE_DIR}/Toastr.cpp)
target_include_directories(imguiToastr PUBLIC ${imguitoastr_SOURCE_DIR})
target_link_libraries(imguiToastr PUBLIC imgui::imgui)

# ========== FetchContent ==========

FetchContent_Declare(
        imguitexteditor
        GIT_REPOSITORY https://github.com/goossens/ImGuiColorTextEdit
        GIT_TAG master
        SOURCE_SUBDIR _none
)
FetchContent_MakeAvailable(imguitexteditor)

# ========== FetchContent ==========

FetchContent_Declare(
        EnTT
        GIT_REPOSITORY https://github.com/skypjack/entt
        GIT_TAG v3.16.0
)
FetchContent_MakeAvailable(EnTT)

# ========== FetchContent ==========

add_library(imguiTextEdit STATIC
        ${imguitexteditor_SOURCE_DIR}/TextDiff.cpp
        ${imguitexteditor_SOURCE_DIR}/TextEditor.cpp
        ${imguitexteditor_SOURCE_DIR}/extras/LspBridge.cpp
        ${imguitexteditor_SOURCE_DIR}/extras/TrieAutoComplete.cpp
)
target_include_directories(imguiTextEdit PUBLIC ${imguitexteditor_SOURCE_DIR})
target_link_libraries(imguiTextEdit PUBLIC imgui::imgui lsp)

# ========== WildFoxEngine ==========
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

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g -fno-omit-frame-pointer)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
        COMMENT "Copying assets to build directory"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

if(WFE_PORTABLE)
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
        GLM_FORCE_XYZW_ONLY
)

# ========== Install / package (Linux portable release) ==========
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
