add_library(LogModule SHARED
    log.cpp
)
target_include_directories(LogModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
PRIVATE
)
target_link_libraries(LogModule
PUBLIC
    SDL3::SDL3
)