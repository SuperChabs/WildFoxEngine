cmake_minimum_required(VERSION 3.16)
project(lua LANGUAGES C)

set(CMAKE_C_STANDARD 99)

set(LUA_CORE
    src/lapi.c
    src/lcode.c
    src/lctype.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/llex.c
    src/lmem.c
    src/lobject.c
    src/lopcodes.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/ltable.c
    src/ltm.c
    src/lundump.c
    src/lvm.c
    src/lzio.c
)

set(LUA_LIB
    src/lauxlib.c
    src/lbaselib.c
    src/lcorolib.c
    src/ldblib.c
    src/liolib.c
    src/lmathlib.c
    src/loslib.c
    src/ltablib.c
    src/lstrlib.c
    src/lutf8lib.c
    src/loadlib.c
    src/linit.c
)

add_library(lua STATIC
    ${LUA_CORE}
    ${LUA_LIB}
)

target_include_directories(lua PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(UNIX)
    target_link_libraries(lua PUBLIC m)
endif()
