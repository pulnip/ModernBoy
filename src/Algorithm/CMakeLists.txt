file(GLOB ALGORITHM_SRC "*.cpp")

add_library(algorithm_impl
    ${ALGORITHM_SRC}
)

target_include_directories(algorithm_impl PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/directxtk"
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/directxmath"
)

find_package(gsl-lite CONFIG REQUIRED)
if(gsl-lite_FOUND)
    target_link_libraries(algorithm_impl PRIVATE
        gsl::gsl-lite-v1
    )
endif()
