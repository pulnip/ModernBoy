add_subdirectory(${BACKEND_SOURCE_DIR})

file(GLOB ALL_SOURCES "*.cpp")
add_library(RenderModule STATIC ${ALL_SOURCES})

target_compile_options(RenderModule PRIVATE
    -Wall -Wextra
)
target_compile_definitions(RenderModule PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(RenderModule PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(RenderModule PRIVATE
    # SDL3
    SDL3::SDL3
)