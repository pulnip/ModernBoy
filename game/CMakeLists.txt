file(GLOB ALL_SOURCES "*.cpp")
add_library(Game SHARED
    ${ALL_SOURCES}
)
target_compile_options(Game PRIVATE
PRIVATE
    $<IF:$<CONFIG:DEBUG>, -Wall -Wextra,>
)
target_link_libraries(Game
PUBLIC
    Core
    Engine
)