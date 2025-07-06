file(GLOB ALL_SOURCES "*.cpp")
add_library(UIModule STATIC
    ${ALL_SOURCES}
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdl3.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_metal.mm
)

target_compile_options(UIModule PRIVATE
    -Wall -Wextra
    # -fsanitize=address
)
target_compile_definitions(UIModule PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(UIModule PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    # ImGui
    ${ImGui_INCLUDE_DIRS}
    # metal-cpp
    ${CMAKE_SOURCE_DIR}/external/metal-cpp
)

target_link_libraries(UIModule PRIVATE
    # SDL3
    SDL3::SDL3
    # ImGui
    ImGui
    # metal-cpp
    "-framework Metal"
    "-framework MetalKit"
    "-framework Foundation"
    "-framework QuartzCore"
)