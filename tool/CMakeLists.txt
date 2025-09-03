add_executable(MeshConverter
    mesh_converter.cpp
)
target_link_libraries(MeshConverter
PRIVATE
    $<IF:$<CONFIG:DEBUG>, sanitizer,>
    Core
    AssetModule
)

add_executable(MeshViewer
    mesh_viewer.cpp
)
target_link_libraries(MeshViewer
PRIVATE
    $<IF:$<CONFIG:DEBUG>, sanitizer,>
    Core
    AssetModule
)
