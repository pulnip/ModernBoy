file(GLOB TEST_SRC "*.cpp")

add_executable(Test
    ${TEST_SRC}
)

target_include_directories(Test PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

find_package(GTest CONFIG REQUIRED)
if(GTest_FOUND)
    message(STATUS "Using GTest package")
    target_link_libraries(Test PRIVATE
        GTest::gtest    
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(Test)
endif()

add_test(NAME ${PROJECT_NAME}_test
    COMMAND Test
)

set_tests_properties(${PROJECT_NAME}_test
PROPERTIES
    TIMEOUT 10
)
