enable_language(Swift)

add_library(RenderBackend STATIC
  context.cpp
  mesh.cpp
  shader.cpp
)
add_library(MetalContext SHARED
  context.swift
  mesh.swift
  shader.swift
)

target_compile_options(RenderBackend PRIVATE
  -Wall -Wextra
)
target_compile_definitions(RenderBackend PRIVATE
  $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(RenderBackend PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  # gsl-lite
  ${gsl-lite_INCLUDE_DIRS}
  # ImGui
  ${ImGui_INCLUDE_DIRS}
)
target_link_libraries(RenderBackend PRIVATE
  MetalContext
  # SDL3
  SDL3::SDL3
  # SDL3_image
  SDL3_image::SDL3_image
  # ImGui
  ${ImGui_LIBS}
  "-framework Metal"
  "-framework Foundation"
)