# Library options
option(ImGui_DEMO
  "Include the ImGui demo window implementation in library"
  ON
)

find_path(ImGui_INCLUDE_DIR
  NAMES imgui.h
  PATHS ${CMAKE_SOURCE_DIR}/vendor/imgui
)
if(NOT ImGui_INCLUDE_DIR)
  message(FATAL_ERROR "ImGui header Not Found!")
else()
  set(ImGui_FOUND TRUE)
  message(STATUS "ImGui found. Headers: ${ImGui_INCLUDE_DIR}")
endif()

set(ImGui_SRC_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
set(ImGui_SOURCES
  ${ImGui_SRC_DIR}/imgui.cpp
  ${ImGui_SRC_DIR}/imgui_draw.cpp
  ${ImGui_SRC_DIR}/imgui_tables.cpp
  ${ImGui_SRC_DIR}/imgui_widgets.cpp
  $<$<BOOL:ImGui_DEMO>:${ImGui_SRC_DIR}/imgui_demo.cpp>
  ${ImGui_SRC_DIR}/backends/imgui_impl_sdl3.cpp
)

if(ImGui_RENDERER_BACKEND STREQUAL "DirectX")
  list(APPEND ImGui_SOURCES
    ${ImGui_SOURCES}
    ${ImGui_SRC_DIR}/backends/imgui_impl_dx11.cpp
  )
elseif(ImGui_RENDERER_BACKEND STREQUAL "OpenGL")
  list(APPEND ImGui_SOURCES
    ${ImGui_SOURCES}
    ${ImGui_SRC_DIR}/backends/imgui_impl_opengl3.cpp
  )
endif()

set(ImGui_INCLUDE_DIRS
  ${ImGui_INCLUDE_DIR}
  ${ImGui_INCLUDE_DIR}/backends
)

add_library(ImGui STATIC
  ${ImGui_SOURCES}
)
target_compile_definitions(ImGui PRIVATE
  $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)
target_include_directories(ImGui PRIVATE
  ${ImGui_INCLUDE_DIRS}
)

set(ImGui_LIBS ImGui)

mark_as_advanced(ImGui_DEMO ImGui_INCLUDE_DIR ImGui_SOURCES ImGui_SRC_DIR)