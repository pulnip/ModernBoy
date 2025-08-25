add_subdirectory(${BACKEND_SOURCE_DIR})

find_path(KTX_INCLUDE_DIR
    NAMES ktx.h
    HINTS /usr/local/include
)
find_library(KTX_LIB
    NAMES ktx
    PATHS /usr/local/lib
)

file(GLOB ALL_SOURCES "*.cpp")
add_library(RenderModule STATIC ${ALL_SOURCES})

target_compile_options(RenderModule PRIVATE
    -Wall -Wextra
)
target_compile_definitions(RenderModule PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(RenderModule
PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${KTX_INCLUDE_DIR}
)
target_link_libraries(RenderModule PRIVATE
    # SDL3
    SDL3::SDL3
    ${KTX_LIB}
)