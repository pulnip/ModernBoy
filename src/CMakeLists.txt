add_subdirectory(backends)

file(GLOB ALL_SOURCES "*.cpp")

add_executable(${PROJECT_NAME}
  ${ALL_SOURCES}
)
add_dependencies(${PROJECT_NAME} Shaders)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall -Wextra
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  # gsl-lite
  ${gsl-lite_INCLUDE_DIRS}
  # ImGui
  ${ImGui_INCLUDE_DIRS}
  # angelscript
  ${CMAKE_SOURCE_DIR}/external/angelscript/include
  # toml++
  ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
  # stb
  ${CMAKE_SOURCE_DIR}/external/stb
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  RenderBackend
  # SDL3
  SDL3::SDL3
  # SDL3_image
  SDL3_image::SDL3_image
  # ImGui
  ${ImGui_LIBS}
  # angelscript
  angelscript
  assimp::assimp
)