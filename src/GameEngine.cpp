#include <algorithm>
#include <cassert>

#include "PubSubMessage.hpp"
#include "GameEngine/GameEngine.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/ResourceManager.hpp"
#include "SubEngine/InputSystem.hpp"
#include "SubEngine/Timer.hpp"

void GameEngine::postConstruct() noexcept{
    injectDependency();
}

void GameEngine::run() noexcept {
    timer->reset();

    while (gameLogic->isReady()) {
        // 마지막 프레임 이후로 최소한 16ms가 경과할 때까지 대기
        timer->wait(16);

        // 최대 델타 시간 값은 0.05s
        auto deltaTime = std::min(0.05f, timer->getDeltaTime() / 1000.0f);

        // process input
        inputSystem->update(deltaTime);

        // update game
        gameLogic->update(deltaTime);
        // update actor
        actorManager->update(deltaTime);

        // generate output
        graphicsEngine->update(deltaTime);
    }
}

std::shared_ptr<SubEngine>
GameEngine::find(const SubEngineName name) noexcept{
    switch(name){
    case SubEngineName::ResourceManager:
        return resourceManager;
    case SubEngineName::InputSystem:
        return inputSystem;
    case SubEngineName::GameLogic:
        return gameLogic;
    case SubEngineName::ActorManager:
        return actorManager;
    case SubEngineName::GraphicsEngine:
        return graphicsEngine;
    default:
        assert(false);
    }
}

void GameEngine::onNotify(MSG_t msg, spObservable se) noexcept{
    switch(msg){
    case Lifetime::CONSTRUCTED:
        switch(se->getName()){
        case SubEngineName::ActorManager:
            actorManager=std::static_pointer_cast<ActorManager>(se);
            break;
        case SubEngineName::GameLogic:
            gameLogic=std::static_pointer_cast<GameLogic>(se);
            break;
        case SubEngineName::GraphicsEngine:
            graphicsEngine=std::static_pointer_cast<GraphicsEngine>(se);
            break;
        case SubEngineName::InputSystem:
            inputSystem=std::static_pointer_cast<InputSystem>(se);
            break;
        case SubEngineName::ResourceManager:
            resourceManager=std::static_pointer_cast<ResourceManager>(se);
            break;;
        case SubEngineName::SoundEngine:
            [[fallthrough]];
        default:
            assert(false);
        }
    break;
    case Lifetime::DESTRUCTED:
        switch(se->getName()){
        case SubEngineName::ActorManager:
            actorManager=nullptr;
            break;
        case SubEngineName::GameLogic:
            gameLogic=nullptr;
            break;
        case SubEngineName::GraphicsEngine:
            graphicsEngine=nullptr;
            break;
        case SubEngineName::InputSystem:
            inputSystem=nullptr;
            break;
        case SubEngineName::ResourceManager:
            resourceManager=nullptr;
            break;;
        case SubEngineName::SoundEngine:
            [[fallthrough]];
        default:
            assert(false);
        }
    break;
    default:
        assert(false);
    }
}
