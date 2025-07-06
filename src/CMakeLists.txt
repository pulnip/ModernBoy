add_subdirectory(${BACKEND_SOURCE_DIR})
add_subdirectory(common)
add_subdirectory(game)
add_subdirectory(input)
add_subdirectory(render)
add_subdirectory(shader)
add_subdirectory(script)
add_subdirectory(util)
add_subdirectory(ui)

file(GLOB ALL_SOURCES "*.cpp")
add_executable(${PROJECT_NAME}
    ${ALL_SOURCES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra
    # -fsanitize=address
)
target_link_options(${PROJECT_NAME} PRIVATE
    # -fsanitize=address
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(${PROJECT_NAME} PUBLIC
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
target_link_libraries(${PROJECT_NAME} PRIVATE
    CommonModule
    Game
    RenderBackend
    RenderModule
    InputModule
    ScriptModule
    Util
    UIModule
    # SDL3
    SDL3::SDL3
    # SDL3_image
    SDL3_image::SDL3_image
    # ImGui
    ${ImGui_LIBS}
    # Assimp
    assimp::assimp
)