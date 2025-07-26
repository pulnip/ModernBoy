cmake_minimum_required(VERSION 3.28)

project(ModernBoy LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

option(BUILD_TEST_TEMPLATE "Ignore warnings related to TODOs" ON)
option(ENABLE_CODE_ANALYSIS "Use Static Code Analysis on build" ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_PREFIX_PATH ${CMAK_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(cmake/config.cmake)

find_package(RapidJSON REQUIRED CONFIG)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG)
set(ImGui_RENDER_BACKEND ${RENDER_BACKEND})
find_package(ImGui REQUIRED)
find_package(gsl-lite REQUIRED)
find_package(Assimp REQUIRED CONFIG)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF
    CACHE INTERNAL "Suppress dev warnings for external projects")
add_subdirectory(external/angelscript/sdk/angelscript/projects/cmake)
target_compile_options(angelscript PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-w>
    $<$<CXX_COMPILER_ID:MSVC>:/w>
)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON
    CACHE INTERNAL "Restore dev warnings")

add_subdirectory(src)
# add_subdirectory(src/network/posix)

enable_testing()
add_subdirectory(test)
