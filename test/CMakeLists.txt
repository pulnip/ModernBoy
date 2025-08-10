find_package(GTest CONFIG REQUIRED)
file(GLOB ALL_SOURCES "*.cpp")
add_executable(${PROJECT_NAME}Test
    ${ALL_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/engine/physics/physics.cpp
    ${CMAKE_SOURCE_DIR}/src/game/entity_registry.cpp
)

target_compile_options(${PROJECT_NAME}Test
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(${PROJECT_NAME}Test
PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
    DISABLE_LOG
)

target_link_libraries(${PROJECT_NAME}Test
PRIVATE
    Core
    GTest::gtest_main
    GTest::gmock_main
    SDL3::SDL3
)

include(GoogleTest)
add_test(
    NAME ${PROJECT_NAME}-Test
    COMMAND $<TARGET_FILE:${PROJECT_NAME}Test>
)