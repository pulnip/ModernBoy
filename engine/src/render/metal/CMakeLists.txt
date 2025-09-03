set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0")
enable_language(Swift)
file(GLOB SWIFT_SOURCES "*.swift")
add_library(MetalContext SHARED ${SWIFT_SOURCES})
target_compile_options(MetalContext
PRIVATE
    "-import-objc-header" ${CMAKE_SOURCE_DIR}/engine/include/asset/ktx2_transcode.h
)

file(GLOB CPP_SOURCES "*.cpp")
add_library(RenderBackend SHARED ${CPP_SOURCES})
target_compile_options(RenderBackend
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(RenderBackend
PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)
target_include_directories(RenderBackend
PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/include
PRIVATE
    # gsl-lite
    ${gsl-lite_INCLUDE_DIRS}
)
target_link_libraries(RenderBackend
PUBLIC
    Core
    LogModule
    MetalContext
PRIVATE
    # SDL3
    SDL3::SDL3
)