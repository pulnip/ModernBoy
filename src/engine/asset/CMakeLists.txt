add_library(AssetModule SHARED
    mesh_importer.cpp
    scene_parser.cpp
)
target_include_directories(AssetModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
)
target_link_libraries(AssetModule
PRIVATE
    Core
    LogModule
    assimp::assimp
)