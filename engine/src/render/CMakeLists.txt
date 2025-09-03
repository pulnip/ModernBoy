add_subdirectory(core)
add_subdirectory(${BACKEND_SOURCE_DIR})

add_library(RenderModule INTERFACE)

target_link_libraries(RenderModule
INTERFACE
    RenderCore
    RenderBackend
)
