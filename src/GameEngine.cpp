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

void GameEngine::onNotify(Lifetime msg, std::shared_ptr<SubEngine> se) noexcept{
    std::shared_ptr<ISubEngine> ise=se;
    switch(msg){
    case Lifetime::CONSTRUCTED:
        subEngines.emplace(ise->getType(), ise);
        break;
    case Lifetime::DESTRUCTED:
        subEngines.erase(ise->getType());
        break;
    default:
        assert(false);
    }
}

void GameEngine::handler() noexcept{
    isRunning=false;
}

struct Essentials{
    using SE=std::shared_ptr<ISubEngine>;
    SE actorManager;
    SE gameLogic;
    SE graphicsEngine;
    SE inputSystem;
    SE physicsSimulator;
    SE resourceManager;
    int set(const GameEngine::SubEngineMap& map) noexcept;
};

int Essentials::set(const GameEngine::SubEngineMap& map) noexcept{
    int countEssentials=0;

    for(auto &[key, se]: map){
        switch(key){
        case SubEngine::Type::ActorManager:
            actorManager=se;
            countEssentials+=1;
            break;
        case SubEngine::Type::GameLogic:
            gameLogic=se;
            countEssentials+=1;
            break;
        case SubEngine::Type::GraphicsEngine:
            graphicsEngine=se;
            countEssentials+=1;
            break;
        case SubEngine::Type::InputSystem:
            inputSystem=se;
            countEssentials+=1;
            break;
        case SubEngine::Type::PhysicsSimulator:
            physicsSimulator=se;
            countEssentials+=1;
            break;
        case SubEngine::Type::ResourceManager:
            resourceManager=se;
            countEssentials+=1;
            break;
        }
    }

    return countEssentials;
}

void GameEngine::run() noexcept {
    Essentials e;
    bool completeness=e.set(subEngines)==6;
    assert(completeness);
    isRunning = completeness;

    timer->reset();

    while (isRunning) {
        // 마지막 프레임 이후로 최소한 16ms가 경과할 때까지 대기
        timer->wait(16);

        // 최대 델타 시간 값은 0.05s
        auto deltaTime = std::min(0.05f, timer->getDeltaTime() / 1000.0f);

        // process input
        e.inputSystem->update(deltaTime);

        // update game
        e.gameLogic->update(deltaTime);
        
        e.physicsSimulator->update(deltaTime);
        // update actor
        e.actorManager->update(deltaTime);

        // generate output
        e.graphicsEngine->update(deltaTime);
    }
}

std::shared_ptr<ISubEngine>
GameEngine::find(const SubEngine::Type name) noexcept{
    auto result=subEngines.find(name);
    if(result==subEngines.end()){
        return nullptr;
    }
    return result->second;
}
