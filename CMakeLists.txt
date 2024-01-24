cmake_minimum_required(VERSION 3.20.0)
project(GameEngineDevelopment VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_compile_options(-fmodules-ts)

add_executable(${PROJECT_NAME}
    main.cpp

    src/Math.cpp
    src/GameEngine.cpp
    src/SubEngine.cpp
    src/ActorManager.cpp
    src/GraphicsEngine.cpp
    src/GameLogic.cpp
    src/InputSystem.cpp
    src/PhysicsSimulator.cpp
    src/Timer_default.cpp
    src/Actor.cpp
    src/Component.cpp
    src/DrawComponent.cpp
    src/BoxComponent.cpp
    src/MoveComponent.cpp
    src/InputComponent.cpp

    GameEngineWithSDL/src/GameEngineWithSDL.cpp
    GameEngineWithSDL/src/GraphicsEngineWithSDL.cpp
    GameEngineWithSDL/src/InputSystemWithSDL.cpp
    GameEngineWithSDL/src/ResourceManagerWithSDL.cpp
    GameEngineWithSDL/src/SpriteComponent.cpp
    GameEngineWithSDL/src/AnimSpriteComponent.cpp
    GameEngineWithSDL/src/BGSpriteComponent.cpp
    GameEngineWithSDL/src/TimerWithSDL.cpp

    Game/src/TestGame.cpp
    Game/src/ActorManagerForTest.cpp
    Game/src/Ball.cpp
    Game/src/Paddle.cpp
    Game/src/Wall.cpp
    Game/src/Asteroid.cpp
    Game/src/CelestialSky.cpp
    Game/src/Ship.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/GameEngineWithSDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Game/include
)

target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_image::SDL2_image
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
