cmake_minimum_required(VERSION 3.30)

project(WildFoxEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_MODULE_STD OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fmodules-ts)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

# ========== Glad =============
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

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
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
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

# ========== Magic_enum =============
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${CMAKE_SOURCE_DIR}/vendor/magic_enum/include)

# ========== ENTT =============
add_subdirectory(vendor/entt) 

# ========== nlohmann_json =============
add_library(json INTERFACE)
target_include_directories(json INTERFACE ${CMAKE_SOURCE_DIR}/vendor/nlohmann_json/include)

# ========== Lua C API =============
file(GLOB LUA_SOURCES "${CMAKE_SOURCE_DIR}/vendor/lua/lua-5.4.8/src/*.c")
list(REMOVE_ITEM LUA_SOURCES 
    "${CMAKE_SOURCE_DIR}/vendor/lua/lua-5.5.0/src/lua.c" 
    "${CMAKE_SOURCE_DIR}/vendor/lua/lua-5.5.0/src/luac.c"
)

add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${CMAKE_SOURCE_DIR}/vendor/lua/lua-5.4.8/src)

# ========== sol2 =============
add_subdirectory(vendor/sol2 EXCLUDE_FROM_ALL)

# ========== nfd =============
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
#
#add_library(nfd STATIC
#    vendor/nfd/src/nfd_common.c
#    vendor/nfd/src/nfd_gtk.c
#)
#
#target_include_directories(nfd PUBLIC
#    vendor/nfd/src
#    ${GTK3_INCLUDE_DIRS}
#)
#
#target_link_libraries(nfd PRIVATE
#    ${GTK3_LIBRARIES}
#)
#
#target_compile_options(nfd PRIVATE
#    ${GTK3_CFLAGS_OTHER}
#)

# ========== WildFoxEngine =============
include_directories(${IMGUI_INCLUDE_DIRS})

file(GLOB_RECURSE WildFoxEngine_ALL_SOURCES 
    "src/*.cpp"
    "src/*.c"
)

list(FILTER WildFoxEngine_ALL_SOURCES EXCLUDE REGEX ".*/old/.*")

file(GLOB_RECURSE WildFoxEngine_ALL_HEADERS 
    "src/*.h"
    "src/*.hpp"
)
list(FILTER WildFoxEngine_ALL_HEADERS EXCLUDE REGEX ".*/old/.*")

file(GLOB_RECURSE WildFoxEngine_MODULES 
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

message(STATUS "=== Founded files ===")
message(STATUS "C++ files: ${WildFoxEngine_SOURCES}")
message(STATUS "Headers: ${WildFoxEngine_ALL_HEADERS}")

add_executable(${PROJECT_NAME} 
    ${WildFoxEngine_SOURCES}
    ${WildFoxEngine_ALL_HEADERS}
)

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
        CXX_MODULE_STD ""
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src   
)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    glad
    glm::glm
    stb_image
    assimp
    imgui
    imguizmo
    magic_enum
    EnTT::EnTT
    #nfd
    lua
    sol2::sol2
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets to build directory"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ASSETS_PATH="${CMAKE_BINARY_DIR}/assets"
    PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
    SOL_ALL_SAFETIES_ON=1
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)