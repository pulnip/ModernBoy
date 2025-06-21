message(STATUS "Backend dir: ${BACKEND_SOURCE_DIR}")
add_subdirectory(${BACKEND_SOURCE_DIR})
add_subdirectory(input)
add_subdirectory(shader)
add_subdirectory(util)

file(GLOB ALL_SOURCES "*.cpp")
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

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
    # toml++
    ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
    # stb
    ${CMAKE_SOURCE_DIR}/external/stb
    # angelscript
    ${CMAKE_SOURCE_DIR}/external/angelscript/sdk/angelscript/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Util
    RenderBackend
    InputModule
    # SDL3
    SDL3::SDL3
    # SDL3_image
    SDL3_image::SDL3_image
    # ImGui
    ${ImGui_LIBS}
    # Assimp
    assimp::assimp
)