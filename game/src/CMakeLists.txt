file(GLOB ALL_SOURCES "*.cpp")
add_library(Game SHARED
    ${ALL_SOURCES}
)
target_compile_options(Game
PRIVATE
    $<IF:$<CONFIG:DEBUG>, -Wall -Wextra,>
)
target_include_directories(Game
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(Game
PUBLIC
    Core
    Engine
)