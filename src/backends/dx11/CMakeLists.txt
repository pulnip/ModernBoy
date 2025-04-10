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
  $<IF:$<CONFIG:DEBUG>,
    D3DCOMPILE_DEBUG D3DCOMPILE_SKIP_OPTIMIZATION
    D3DCOMPILE_ENABLE_STRICTNESS,
  >
)

target_include_directories(RenderBackend PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  # DirectXMath
  C:/tools/msys64/mingw64/include/directxmath
  ${Backend_INCLUDE_DIRS}
  # gsl-lite
  ${gsl-lite_INCLUDE_DIRS}
  # ImGui
  ${ImGui_INCLUDE_DIRS}
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
  # DirectX11
  d3d11.lib d3dcompiler.lib dxgi.lib dxguid.lib
  # DirectXMath
  Microsoft::DirectXMath
)