cmake_minimum_required(VERSION 3.16)

project(XEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_library(stb_image STATIC vendor/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC vendor/stb_image)

include_directories(${CMAKE_SOURCE_DIR}/includes)

file(GLOB_RECURSE SOURCES "src/*.cpp")

# Ensure top-level main.cpp (project entry) is included
# list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    glad
    glm::glm
    stb_image
    assimp
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_BINARY_DIR}/assets
)

add_compile_definitions(ASSETS_PATH="${CMAKE_BINARY_DIR}/assets")

# Встановлення робочої директорії для виконання
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
