cmake_minimum_required(VERSION 3.28)

project(ModernBoy
    VERSION 0.0.1
    LANGUAGES C CXX
)

option(BUILD_TEST_TEMPLATE "Ignore warnings related to TODOs" OFF)
option(ENABLE_CODE_ANALYSIS "Use Static Code Analysis on build" OFF)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
