cmake_minimum_required(VERSION 3.28)

project("slispWASM")
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Output directory for deployment
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/htmlOut")
file(MAKE_DIRECTORY ${OUTPUT_DIR})

# Emscripten output config
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

add_executable(engineWASM
    ../src/node.cpp
    ../src/engine.cpp
    ../src/ondemandistream.cpp
    ../src/toker.cpp
    ../src/token.cpp
    ../src/parsey.cpp
    ../src/fundamental.cpp
    ../src/scopey.cpp
    ../src/interpreter.cpp
    src/emsBindings.cpp
)

target_link_options(engineWASM PRIVATE
    "-s WASM=1"
    "-s MODULARIZE=1"
    "-s ALLOW_MEMORY_GROWTH=1"
    "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
    "--bind"
    "-o ${OUTPUT_DIR}/engineWASM.js"
)

# Post-build: Copy HTML
add_custom_command(TARGET engineWASM POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/html/index.html"
        "${OUTPUT_DIR}/index.html"
    COMMENT "Copying index.html to htmlOut/"
)
