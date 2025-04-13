find_path(gsl-lite_INCLUDE_DIR
  NAMES gsl-lite/gsl-lite.hpp
  PATHS ${CMAKE_SOURCE_DIR}/external/gsl-lite/include
)
if(NOT gsl-lite_INCLUDE_DIR)
  message(FATAL_ERROR "gsl-lite header Not Found!")
else()
  set(gsl-lite_FOUND TRUE)
  message(STATUS "gsl-lite found. Headers: ${gsl-lite_INCLUDE_DIR}")
endif()

set(gsl-lite_INCLUDE_DIRS
  ${gsl-lite_INCLUDE_DIR}
)