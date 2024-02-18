#include <algorithm>
#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/InputSystem.hpp"
#include "SubEngine/Logger.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "SubEngine/ResourceManager.hpp"
#include "SubEngine/SoundEngine.hpp"
#include "SubEngine/Timer.hpp"

using namespace Game;
using namespace Game::Core;

void Engine::run() noexcept {
    isRunning = true;

    timer->reset();

    while (isRunning) {
        // 마지막 프레임 이후로 최소한 16ms가 경과할 때까지 대기
        timer->wait(16);

        // 최대 델타 시간 값은 0.05s
        auto deltaTime = std::min(
            0.05, timer->getDeltaTime()/1000.0
        );

        // process input
        inputSystem->update(deltaTime);

        // update game
        gameLogic->update(deltaTime);
        physicsSimulator->update(deltaTime);
        actorManager->update(deltaTime);

        // generate output
        graphicsEngine->update(deltaTime);
        soundEngine->update(deltaTime);
    }
}

Plugin Engine::find(const SubEngine::Type type) noexcept{
    switch(type){
    case SubEngine::Type::Logger:
        return logger;
    case SubEngine::Type::ResourceManager:
        return resourceManager;
    case SubEngine::Type::InputSystem:
        return inputSystem;
    case SubEngine::Type::GameLogic:
        return gameLogic;
    case SubEngine::Type::PhysicsSimulator:
        return physicsSimulator;
    case SubEngine::Type::ActorManager:
        return actorManager;
    case SubEngine::Type::GraphicsEngine:
        return graphicsEngine;
    case SubEngine::Type::SoundEngine:
        return soundEngine;
    default:
        assert(false);
    }
}

void Engine::handler() noexcept{
    isRunning=false;
}
