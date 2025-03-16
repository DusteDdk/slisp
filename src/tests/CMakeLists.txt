
set(TEST_TARGET_NAME UnitTests)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR})

find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
add_executable(${TEST_TARGET_NAME}
    test-toker.cpp
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE toker)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE Catch2::Catch2WithMain)

set(UNIT_TEST ${TEST_TARGET_NAME})
add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})


# add_custom_command(
#      TARGET ${UNIT_TEST}
#      COMMENT "Runing tests"
#      POST_BUILD
#      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#      COMMAND ${TEST_TARGET_NAME} -s
# )


include(Catch)
catch_discover_tests(${TEST_TARGET_NAME})