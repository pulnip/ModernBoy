if(RENDERER_BACKEND STREQUAL "DirectX")
  list(APPEND TARGET_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/dx11_adaptor.cpp
  )
  list(APPEND TARGET_COMPILE_DEFS
    $<IF:$<CONFIG:DEBUG>,
      D3DCOMPILE_DEBUG D3DCOMPILE_SKIP_OPTIMIZATION
      D3DCOMPILE_ENABLE_STRICTNESS,
    >
  )

  list(APPEND TARGET_INCLUDE_DIRS
    # DirectXMath
    C:/tools/msys64/mingw64/include/directxmath
    # DirectXTK
    C:/tools/msys64/mingw64/include/directxtk
  )
  list(APPEND TARGET_LINK_LIBS
    # DirectX11
    d3d11 d3dcompiler dxgi
    # DirectXMath
    Microsoft::DirectXMath
    # DirectXTK
    d3d11.lib dxgi.lib dxguid.lib uuid.lib
    kernel32.lib user32.lib
    comdlg32.lib advapi32.lib shell32.lib
    ole32.lib oleaut32.lib
    Microsoft::DirectXTK
  )
elseif(RENDERER_BACKEND STREQUAL "OpenGL")
  list(APPEND TARGET_INCLUDE_DIRS
    # glad
    ${glad_INCLUDE_DIRS}
  )
  list(APPEND TARGET_LINK_LIBS
    # glad
    ${glad_LIBS}
  )
endif()

file(GLOB ALL_SOURCES "*.cpp")

add_executable(${PROJECT_NAME}
  ${ALL_SOURCES}
  ${TARGET_SOURCES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall -Wextra
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
  ${TARGET_COMPILE_DEFS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  # gsl-lite
  ${gsl-lite_INCLUDE_DIRS}
  # ImGui
  ${ImGui_INCLUDE_DIRS}
  # target-specific dirs
  ${TARGET_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  stdc++exp
  # SDL3
  SDL3::SDL3
  # SDL3_image
  SDL3_image::SDL3_image
  # ImGui
  ${ImGui_LIBS}
  # target-specific libs
  ${TARGET_LINK_LIBS}
)