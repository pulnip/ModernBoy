file(GLOB ALL_SOURCES "*.cpp")
add_library(InputModule SHARED
    ${ALL_SOURCES}
)

target_compile_options(InputModule
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(InputModule
PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(InputModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/include
)
target_link_libraries(InputModule
PUBLIC
    Core
    InterfaceModule
PRIVATE
    # SDL3
    SDL3::SDL3
)