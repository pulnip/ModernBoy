add_library(RenderBackend STATIC
  context.cpp
  mesh.cpp
  shader.cpp
)

target_compile_options(RenderBackend PRIVATE
  -Wall -Wextra
)
target_compile_definitions(RenderBackend PRIVATE
  $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(RenderBackend PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  # gsl-lite
  ${gsl-lite_INCLUDE_DIRS}
  # ImGui
  ${ImGui_INCLUDE_DIRS}
  # glad
  ${glad_INCLUDE_DIRS}
)
target_link_libraries(RenderBackend PRIVATE
  stdc++exp
  TBB::tbb
  # SDL3
  SDL3::SDL3
  # SDL3_image
  SDL3_image::SDL3_image
  # ImGui
  ${ImGui_LIBS}
  # glad
  ${glad_LIBS}
)