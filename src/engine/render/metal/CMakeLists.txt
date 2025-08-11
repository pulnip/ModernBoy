set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0")
enable_language(Swift)

file(GLOB CPP_SOURCES "*.cpp")
file(GLOB SWIFT_SOURCES "*.swift")

add_library(RenderBackend STATIC ${CPP_SOURCES})
add_library(MetalContext SHARED ${SWIFT_SOURCES})

target_compile_options(RenderBackend PRIVATE
    -Wall -Wextra
)

target_compile_definitions(RenderBackend PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(RenderBackend PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    # gsl-lite
    ${gsl-lite_INCLUDE_DIRS}
)
target_link_libraries(RenderBackend PRIVATE
    $<IF:$<CONFIG:DEBUG>, sanitizer,>
    MetalContext
    # SDL3
    SDL3::SDL3
)