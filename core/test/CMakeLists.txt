file(GLOB TEST_SOURCES "*.cpp")
add_executable(CoreTest
    ${TEST_SOURCES}
)

target_compile_options(CoreTest
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(CoreTest
PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_link_libraries(CoreTest
PRIVATE
    Core
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
add_test(
    NAME Core-Test
    COMMAND $<TARGET_FILE:CoreTest>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_tests_properties(Core-Test PROPERTIES LABELS "unit;core")