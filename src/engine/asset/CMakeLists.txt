file(GLOB ALL_SOURCES "*.cpp")

add_library(AssetModule SHARED
    ${ALL_SOURCES}
)
target_include_directories(AssetModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
)
target_link_libraries(AssetModule
PUBLIC
    RenderBackend
PRIVATE
    Core
    LogModule
    assimp::assimp
)