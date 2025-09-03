add_subdirectory(src/asset)
add_subdirectory(src/input)
add_subdirectory(src/interface)
add_subdirectory(src/log)
add_subdirectory(src/physics)
add_subdirectory(src/render)
add_subdirectory(src/runtime)
add_subdirectory(src/script)
add_subdirectory(src/ui)

add_library(Engine INTERFACE)
add_library(ModernBoy::Engine ALIAS Engine)

target_link_libraries(Engine
INTERFACE
    EngineRuntime
    AssetModule
    InputModule
    InterfaceModule
    LogModule
    RenderBackend
    RenderModule
    PhysicsModule
    ScriptModule
    UIModule
)

# install(TARGETS Engine
#     EXPORT ModernBoyTargets
# )
