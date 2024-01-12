cmake_minimum_required(VERSION 3.20.0)
project(GameEngineDevelopment VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    Game.cpp
    ActorManager.cpp
    GameLogic.cpp
    GraphicsEngine.cpp
    GraphicsEngineWithSDL.cpp
    InputSystemWithSDL.cpp
    PhysicsSimulator.cpp
    ResourceManagerWithSDL.cpp
    SoundEngine.cpp
    Actors.cpp
    Components.cpp
    Math.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_image::SDL2_image
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
