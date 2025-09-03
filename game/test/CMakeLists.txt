file(GLOB TEST_SOURCES "*.cpp")
add_executable(GameTest
    ${TEST_SOURCES}
)

target_compile_options(GameTest
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(GameTest
PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
    DISABLE_LOG
)

target_link_libraries(GameTest
PRIVATE
    Game
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
add_test(
    NAME Game-Test
    COMMAND $<TARGET_FILE:GameTest>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_tests_properties(Game-Test PROPERTIES LABELS "unit;game")