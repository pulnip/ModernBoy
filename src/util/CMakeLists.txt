file(GLOB ALL_SOURCES "*.cpp")
add_library(Util STATIC ${ALL_SOURCES})

target_compile_options(Util PRIVATE
    -Wall -Wextra
)
target_compile_definitions(Util PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(Util PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(Util PRIVATE
)
