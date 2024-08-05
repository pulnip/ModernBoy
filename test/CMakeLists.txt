enable_testing()

file(GLOB TEST_SRC "*.cpp")

add_executable(Test
    ${TEST_SRC}
)

target_include_directories(Test PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/directxtk"
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/directxmath"
)

target_link_libraries(Test PRIVATE
    algorithm_impl
)

find_package(GTest CONFIG REQUIRED)
if(GTest_FOUND)
    message(STATUS "Using GTest package")
    target_link_libraries(Test PRIVATE
        GTest::gtest    
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
    )

    include(GoogleTest)
    gtest_discover_tests(Test)
endif()

find_package(gsl-lite CONFIG REQUIRED)
if(gsl-lite_FOUND)
    target_link_libraries(Test PRIVATE
        gsl::gsl-lite-v1
    )
endif()
