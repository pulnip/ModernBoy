file(GLOB ALL_SOURCES "*.cpp")

add_library(InterfaceModule SHARED)
target_sources(InterfaceModule
PRIVATE
    ${ALL_SOURCES}
)
target_include_directories(InterfaceModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/include
)
target_link_libraries(InterfaceModule
PUBLIC
    Core
)