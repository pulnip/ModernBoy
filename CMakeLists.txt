cmake_minimum_required (VERSION 3.28)

project(ModernBoy LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

option(BUILD_TEST_TEMPLATE "Ignore warnings related to TODOs" ON)
option(ENABLE_CODE_ANALYSIS "Use Static Code Analysis on build" ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_PREFIX_PATH ${CMAK_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if(WIN32)
  set(DEFAULT_RENDERER "DirectX")
else()
  set(DEFAULT_RENDERER "OpenGL")
endif()

set(RENDERER_BACKEND "${DEFAULT_RENDERER}"
  CACHE STRING "Choose rendering backend (OpenGL, DirectX)"
)
set_property(CACHE RENDERER_BACKEND PROPERTY STRINGS "OpenGL" "DirectX")

if(RENDERER_BACKEND STREQUAL "DirectX")
  if(NOT WIN32)
    message(FATAL_ERROR
      "DirectX is only supported on Windows."
      "Please other backend."
    )
  endif()

  message(STATUS "Using DirectX backend")
  add_definitions(-DUSE_DIRECTX)
  # DirectX related packages
  find_package(directxmath REQUIRED CONFIG)
  find_package(directxtk REQUIRED CONFIG)
elseif(RENDERER_BACKEND STREQUAL "OpenGL")
  message(STATUS "Using OpenGL backend")
  add_definitions(-DUSE_OPENGL)
  # OpenGL related packages
  find_package(OpenGL REQUIRED)
  find_package(glad REQUIRED)
else()
  message(FATAL_ERROR "Invalid RENDERER_BACKEND value.")
endif()

find_package(RapidJSON REQUIRED CONFIG)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG)
set(ImGui_RENDERER_BACKEND ${RENDERER_BACKEND})
find_package(ImGui REQUIRED)
find_package(gsl-lite REQUIRED)

add_subdirectory(src)