set(TARGET_NAME SillyLisp)

find_package(SDL2 REQUIRED)


add_library(fundamental STATIC fundamental.cpp)


add_library(toker STATIC toker.h toker.cpp token.cpp)

add_library(parsey STATIC parsey.h parsey.cpp)
target_link_libraries(parsey PRIVATE toker)



add_library(inter STATIC interpreter.h interpreter.cpp scopey.cpp)
include_directories(inter ${SDL2_INCLUDE_DIRS})

target_link_libraries(inter PRIVATE fundamental ${SDL2_LIBRARIES})

add_library(repl STATIC repl.h repl.cpp)
target_link_libraries(repl PRIVATE toker parsey inter)

add_executable(${TARGET_NAME} main.cpp)

target_link_libraries(${TARGET_NAME} PRIVATE
	repl
	toker
	parsey
	inter
)

add_subdirectory(tests)