add_library(PhysicsModule SHARED)
target_sources(PhysicsModule
PRIVATE
    physics.cpp
)
target_include_directories(PhysicsModule
PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/include
)
target_link_libraries(PhysicsModule
PUBLIC
    Core
)


