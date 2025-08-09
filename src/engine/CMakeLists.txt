add_subdirectory(input)
add_subdirectory(render)
add_subdirectory(script)
add_subdirectory(ui)

file(GLOB ALL_SOURCES "*.cpp")
add_library(Engine STATIC
    ${ALL_SOURCES}
    physics/physics.cpp
    service/debug_draw_service.cpp
    service/input_service.cpp
)

target_compile_options(Engine PRIVATE
    -Wall -Wextra
    -fsanitize=address,leak
)
target_compile_definitions(Engine PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(Engine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    # gsl-lite
    ${gsl-lite_INCLUDE_DIRS}
    # ImGui
    ${ImGui_INCLUDE_DIRS}
    # toml++
    ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
    # stb
    ${CMAKE_SOURCE_DIR}/external/stb
)
target_link_libraries(Engine PRIVATE
    Game
    RenderBackend
    RenderModule
    InputModule
    ScriptModule
    UIModule
    # SDL3
    SDL3::SDL3
    # ImGui
    ${ImGui_LIBS}
    # Assimp
    assimp::assimp
)
