file(GLOB ALGORITHM_SRC "*.cpp")

add_library(algorithm_impl
    ${ALGORITHM_SRC}
)

target_include_directories(algorithm_impl PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)
