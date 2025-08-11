file(GLOB ALL_SOURCES "*.cpp")
add_library(Core SHARED
    math/type.cpp
    memory/bit.cpp
    memory/dynamic_vector.cpp
    string/helper.cpp
)
target_compile_options(Core
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(Core
PUBLIC
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)
target_include_directories(Core
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(Core
PUBLIC
    $<IF:$<CONFIG:DEBUG>, sanitizer,>
)
