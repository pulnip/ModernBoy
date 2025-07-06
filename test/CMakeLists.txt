find_package(GTest CONFIG REQUIRED)
file(GLOB ALL_SOURCES "*.cpp")
add_executable(${PROJECT_NAME}Test
    ${ALL_SOURCES}
)

target_compile_options(${PROJECT_NAME}Test PRIVATE
    -Wall -Wextra
    # -fsanitize=address
)
target_link_options(${PROJECT_NAME}Test PRIVATE
    # -fsanitize=address
)
target_compile_definitions(${PROJECT_NAME}Test PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(${PROJECT_NAME}Test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME}Test PRIVATE
    Util
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
add_test(
    NAME ${PROJECT_NAME}-Test
    COMMAND $<TARGET_FILE:${PROJECT_NAME}Test>
)