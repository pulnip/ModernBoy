file(GLOB ALL_SOURCES "*.cpp")
add_library(InputModule STATIC
    ${ALL_SOURCES}
)

target_compile_options(InputModule PRIVATE
    -Wall -Wextra
    -fsanitize=address
)
target_compile_definitions(InputModule PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(InputModule PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(InputModule PRIVATE
    # SDL3
    SDL3::SDL3
)