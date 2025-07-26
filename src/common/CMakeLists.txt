file(GLOB ALL_SOURCES "*.cpp")
add_library(CommonModule STATIC ${ALL_SOURCES})

target_compile_options(CommonModule PRIVATE
    -Wall -Wextra
    -fsanitize=address,leak
)
target_compile_definitions(CommonModule PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(CommonModule PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
