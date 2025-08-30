file(GLOB ALL_SOURCES "*.cpp")

find_path(KTX_INCLUDE_DIR
    NAMES ktx.h
    HINTS /usr/local/include
)
find_library(KTX_LIB
    NAMES ktx
    PATHS /usr/local/lib
)

add_library(AssetModule SHARED
    ${ALL_SOURCES}
)
target_include_directories(AssetModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
PRIVATE
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/tomlplusplus/include
    ${KTX_INCLUDE_DIR}
)
target_link_libraries(AssetModule
PUBLIC
    RenderBackend
PRIVATE
    Core
    LogModule
    assimp::assimp
    ${KTX_LIB}
)