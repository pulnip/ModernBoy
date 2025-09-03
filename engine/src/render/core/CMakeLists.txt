add_library(RenderCore SHARED)
target_sources(RenderCore
PRIVATE
    renderer.cpp
)
target_compile_options(RenderCore
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(RenderCore
PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)
target_include_directories(RenderCore
PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/include
)
target_link_libraries(RenderCore
PUBLIC
    Core
    InterfaceModule
    LogModule
PRIVATE
    RenderBackend
    SDL3::SDL3
)
