set(TARGET_NAME slisp)

add_library(node STATIC node.cpp)
add_library(bob STATIC bob.cpp)

add_library(ondemandistream STATIC ondemandistream.cpp)

add_library(fundamental STATIC fundamental.cpp)

add_library(toker STATIC toker.h toker.cpp token.cpp)

add_library(parsey STATIC parsey.h parsey.cpp)
target_link_libraries(parsey PRIVATE toker node bob)


find_package(SDL2 REQUIRED)
include_directories(sdlfuncs ${SDL2_INCLUDE_DIRS})
add_library(sdlfuncs STATIC sdlfuncs.cpp)

add_library(inter STATIC interpreter.h interpreter.cpp scopey.cpp)
target_link_libraries(inter PRIVATE fundamental sdlfuncs ${SDL2_LIBRARIES})


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

add_executable(toDot dot.cpp)
target_link_libraries(toDot PRIVATE
	parsey
)

add_subdirectory(tests)

