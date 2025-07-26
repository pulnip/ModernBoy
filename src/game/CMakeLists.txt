file(GLOB ALL_SOURCES "*.cpp")
add_library(Game STATIC
    ${ALL_SOURCES}
)

target_compile_options(Game PRIVATE
    -Wall -Wextra
    -fsanitize=address,leak
)
target_compile_definitions(Game PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(Game PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(Game PRIVATE
    # SDL3
    SDL3::SDL3
)