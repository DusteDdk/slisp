set(TARGET_NAME SillyLisp)

add_library(toker STATIC toker.h toker.cpp token.cpp)

add_library(parsey STATIC parsey.h parsey.cpp)
target_link_libraries(parsey PRIVATE toker)

add_library(inter STATIC interpreter.h interpreter.cpp scopey.cpp)

add_executable(${TARGET_NAME} main.cpp)

target_link_libraries(${TARGET_NAME} PRIVATE
	toker
	parsey
	inter
)

