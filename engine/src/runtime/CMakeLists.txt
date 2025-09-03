file(GLOB ALL_SOURCES "*.cpp")

add_library(EngineRuntime STATIC)
target_sources(EngineRuntime
PRIVATE
    ${ALL_SOURCES}
)
target_include_directories(EngineRuntime
PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/include
)
target_link_libraries(EngineRuntime
PRIVATE
    Core
    Engine
    SDL3::SDL3
)