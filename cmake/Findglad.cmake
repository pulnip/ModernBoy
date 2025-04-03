find_path(glad_INCLUDE_DIR
  NAMES glad/glad.h
  PATHS ${CMAKE_SOURCE_DIR}/vendor/glad/include
)
if(NOT glad_INCLUDE_DIR)
  message(FATAL_ERROR "glad header Not Found!")
else()
  set(glad_FOUND TRUE)
  message(STATUS "glad found. Headers: ${glad_INCLUDE_DIR}")
endif()

set(glad_SRC_DIR ${CMAKE_SOURCE_DIR}/vendor/glad/src)
set(glad_SOURCES
  ${glad_SRC_DIR}/glad.c
)

set(glad_INCLUDE_DIRS
  ${glad_INCLUDE_DIR}
)

add_library(glad STATIC
  ${glad_SOURCES}
)
target_compile_definitions(glad PRIVATE
  $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)
target_include_directories(glad PRIVATE
  ${glad_INCLUDE_DIRS}
)

set(glad_LIBS glad)

mark_as_advanced(glad_INCLUDE_DIR glad_SOURCES glad_SRC_DIR)