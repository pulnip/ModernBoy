file(GLOB ALGORITHM_SRC "*.cpp")

add_library(algorithm_impl
    ${ALGORITHM_SRC}
)

target_include_directories(algorithm_impl PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "$ENV{MSYS_ROOT}/mingw64/include/directxmath"
    "$ENV{MSYS_ROOT}/mingw64/include/directxtk"
)

find_package(gsl-lite CONFIG REQUIRED)
if(gsl-lite_FOUND)
    target_link_libraries(algorithm_impl PRIVATE
        gsl::gsl-lite-v1
    )
endif()
