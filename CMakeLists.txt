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

    src/MainEngine.cpp

    src/Engine/ActorManager.cpp
    src/Engine/Core.cpp
    src/Engine/GameLogic.cpp 
    src/Engine/Graphics.cpp
    src/Engine/InputSystem.cpp
    src/Engine/Logger.cpp
    src/Engine/PhysicsSimulator.cpp
    src/Engine/ResourceManager.cpp
    src/Engine/Timer.cpp
    src/Actor/Vanilla.cpp
    src/Component/Ability.cpp
    src/Component/AI.cpp
    src/Component/Colored.cpp
    src/Component/Controllable.cpp
    src/Component/Drawable.cpp
    src/Component/Movable.cpp

    src/WithSTD/InputSystem.cpp
    src/WithSTD/Timer.cpp

    src/WithSDL/AnimSprite.cpp
    src/WithSDL/BGSprite.cpp
    src/WithSDL/Core.cpp
    src/WithSDL/Graphics.cpp
    src/WithSDL/InputSystem.cpp
    src/WithSDL/Logger.cpp
    src/WithSDL/ResourceManager.cpp
    src/WithSDL/Sprite.cpp
    src/WithSDL/Timer.cpp

    src/Model2D/PhysicsSimulator.cpp

    src/Test/ActorManager.cpp
    src/Test/Asteroid.cpp
    src/Test/Ball.cpp
    src/Test/CelestialSky.cpp
    src/Test/Paddle.cpp
    src/Test/Ship.cpp
    src/Test/Wall.cpp
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
