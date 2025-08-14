add_subdirectory(input)
add_subdirectory(render)
add_subdirectory(script)
add_subdirectory(ui)

file(GLOB ALL_SOURCES "*.cpp")
add_library(Engine STATIC
    ${ALL_SOURCES}
    physics/physics.cpp
    interface/view_service.cpp
    interface/draw_service.cpp
    interface/engine_command_bus.cpp
    interface/input_service.cpp
    asset/mesh_importer.cpp
)

target_compile_options(Engine
PRIVATE
    $<IF:$<CONFIG:DEBUG>, -Wall -Wextra,>
)
target_compile_definitions(Engine
PUBLIC
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(Engine
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
PRIVATE
    # gsl-lite
    ${gsl-lite_INCLUDE_DIRS}
    # ImGui
    ${ImGui_INCLUDE_DIRS}
    # toml++
    ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
    # stb
    ${CMAKE_SOURCE_DIR}/external/stb
)
target_link_libraries(Engine
PRIVATE
    Core
    RenderBackend
    RenderModule
    InputModule
    ScriptModule
    UIModule
    # ImGui
    ${ImGui_LIBS}
    # Assimp
    assimp::assimp
PUBLIC
    $<IF:$<CONFIG:DEBUG>, sanitizer,>
    # SDL3
    SDL3::SDL3
)
