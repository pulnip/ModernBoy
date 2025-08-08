file(GLOB ALL_SOURCES "*.cpp")
add_library(Core STATIC
    math/type.cpp
    memory/bit.cpp
    memory/dynamic_vector.cpp
    string/helper.cpp
)
target_compile_options(Core PRIVATE
    -Wall -Wextra
    -fsanitize=address,leak
)
target_compile_definitions(Core PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)
target_include_directories(Core PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
