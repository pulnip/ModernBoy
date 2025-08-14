add_library(AssetModule SHARED
    mesh_importer.cpp
)
target_include_directories(AssetModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_SOURCE_DIR}/external/stb
)
target_link_libraries(AssetModule
PRIVATE
    assimp::assimp
)