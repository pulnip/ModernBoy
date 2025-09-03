file(GLOB TEST_SOURCES "*.cpp")
add_executable(EngineTest
    ${TEST_SOURCES}
)

target_compile_options(EngineTest
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(EngineTest
PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
    DISABLE_LOG
)

target_link_libraries(EngineTest
PRIVATE
    Engine
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
add_test(
    NAME Engine-Test
    COMMAND $<TARGET_FILE:EngineTest>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_tests_properties(Engine-Test PROPERTIES LABELS "unit;engine")