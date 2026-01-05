cmake_minimum_required(VERSION 3.16)

project(XEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
    # Backend GLFW + OpenGL3
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

# ========== Magic_enum =============
# magic_enum is header-only; provide an INTERFACE target exposing the include dir
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${CMAKE_SOURCE_DIR}/vendor/magic_enum/include)

# ========== XEngine =============
include_directories(${IMGUI_INCLUDE_DIRS})

file(GLOB_RECURSE XENGINE_SOURCES 
    "src/*.cpp"
    "src/*.c"
)

file(GLOB_RECURSE XENGINE_HEADERS 
    "src/*.h"
    "src/*.hpp"
)


add_executable(${PROJECT_NAME} 
    ${XENGINE_SOURCES}
    ${XENGINE_HEADERS}
)

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
    magic_enum
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

add_compile_definitions(
    ASSETS_PATH="${CMAKE_BINARY_DIR}/assets"
    PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
