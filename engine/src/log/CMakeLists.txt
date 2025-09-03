add_library(LogModule SHARED
    log.cpp
)
target_include_directories(LogModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/include
)
target_link_libraries(LogModule
PRIVATE
    SDL3::SDL3
)