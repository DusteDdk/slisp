cmake_minimum_required(VERSION 3.28)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_SCAN_FOR_MODULES 0)

if(UNIX)
    set(CMAKE_CXX_COMPILER             "/usr/bin/clang++-19")
    set(CMAKE_CXX_FLAGS                "-Wall -std=c++23")
    set(CMAKE_CXX_FLAGS_DEBUG          "-g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

    set(CMAKE_AR      "/usr/bin/llvm-ar-19")
    set(CMAKE_LINKER  "/usr/bin/llvm-ld-19")
    set(CMAKE_NM      "/usr/bin/llvm-nm-19")
    set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump-19")
    set(CMAKE_RANLIB  "/usr/bin/llvm-ranlib-19")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

project("slisp")

add_subdirectory(src)
