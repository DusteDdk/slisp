set(TARGET_NAME slisp)

add_library(ondemandistream STATIC ondemandistream.cpp)

add_library(fundamental STATIC fundamental.cpp)

add_library(toker STATIC toker.h toker.cpp token.cpp)

add_library(parsey STATIC parsey.h parsey.cpp)
target_link_libraries(parsey PRIVATE toker)


add_library(sdlfuncs STATIC sdlfuncs.cpp)

if(EMSCRIPTEN)
    message("Using emscripten, SDL2 disabled.")
else()
	find_package(SDL2 REQUIRED)
	include_directories(sdlfuncs ${SDL2_INCLUDE_DIRS})
	target_link_libraries(sdlfuncs PRIVATE fundamental ${SDL2_LIBRARIES})
endif()

add_library(inter STATIC interpreter.h interpreter.cpp scopey.cpp)
target_link_libraries(inter PRIVATE fundamental sdlfuncs)


add_library(engine STATIC engine.cpp)
target_link_libraries(engine PRIVATE
	ondemandistream
	toker
	parsey
	inter
)

add_library(repl STATIC repl.h repl.cpp)
target_link_libraries(repl PRIVATE toker parsey inter ondemandistream)

add_executable(${TARGET_NAME} main.cpp)

target_link_libraries(${TARGET_NAME} PRIVATE
	repl
	toker
	parsey
	inter
)

if(EMSCRIPTEN)
    message("Using emscripten, tests disabled.")


	set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_NAME "index"
    SUFFIX ".html"
    LINK_FLAGS "-s EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
)

else()
	add_subdirectory(tests)
endif()
