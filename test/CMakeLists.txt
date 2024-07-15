file(GLOB TEST_SRC "*.cpp")

add_executable(Test
    ${TEST_SRC}
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
