include(CTest)

file(GLOB TEST_SRC "*.cpp")

add_executable(Test
    ${TEST_SRC}
)

target_include_directories(Test PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${DEV_SYS_INCLUDE}"
    "${DEV_SYS_INCLUDE}/directxmath"
    "${DEV_SYS_INCLUDE}/directxtk"
)

target_link_libraries(Test PRIVATE
    algorithm_impl
)

find_package(GTest CONFIG REQUIRED)
if(GTest_FOUND)
    target_link_libraries(Test PRIVATE
        GTest::gtest    
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
    )

    include(GoogleTest)
    add_test(NAME ${PROJECT_NAME}-Test COMMAND Test)
endif()

find_package(gsl-lite CONFIG REQUIRED)
if(gsl-lite_FOUND)
    target_link_libraries(Test PRIVATE
        gsl::gsl-lite-v1
    )
endif()
