
set(TEST_TARGET_NAME tests)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR})

find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
add_executable(${TEST_TARGET_NAME}
    test-toker.cpp
    test-engine.cpp
)

target_link_libraries(${TEST_TARGET_NAME} PRIVATE
    toker
    engine
    Catch2::Catch2WithMain
)

set(UNIT_TEST ${TEST_TARGET_NAME})
add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

add_executable(engineOnly
    test-stroengine.cpp
)

target_link_libraries(engineOnly PRIVATE
    engine
    Catch2::Catch2WithMain
)


include(Catch)
catch_discover_tests(${TEST_TARGET_NAME})