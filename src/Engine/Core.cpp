#include "Engine/Core.hpp"

#include <algorithm>
#include <SDL2/SDL.h>
#include "Engine/ActorManager.hpp"
#include "Engine/GameLogic.hpp"
#include "Engine/Graphics.hpp"
#include "Engine/InputSystem.hpp"
#include "Engine/Logger.hpp"
#include "Engine/PhysicsSimulator.hpp"
#include "Engine/Timer.hpp"

using namespace Engine;

std::string Engine::title;
Blueprint::Screen Engine::screen;

Core::Core(std::unique_ptr<Timer> timer) noexcept:
    logger(std::make_unique<BindedLogger>("Core", "Base")),
    timer(std::move(timer))
{
    if(this->timer==nullptr){
        logger->error("timer not valid");
    } else{
        logger->debug("valid timer");
    }

    logger->debug("constructed");
}

Core::~Core(){
    logger->debug("destructed");
}

void Core::run() noexcept{
    isRunning = true;

    timer->reset();
    GameLogic::get()->connect();
    InputSystem::get()->connect();

    while (isRunning){
        // 마지막 프레임 이후로 최소한 16ms가 경과할 때까지 대기
        timer->wait(16);

        // 최대 델타 시간 값은 0.05s
        auto deltaTime = std::min(
            0.05, timer->getDeltaTime()/1000.0
        );

        // process input
        if(InputSystem::get()!=nullptr){
            InputSystem::get()->update(deltaTime);
        }

        // update game
        if(GameLogic::get()!=nullptr){
            logger->debug("update GameLogic");
            GameLogic::get()->update(deltaTime);
        } else{
            logger->info("GameLogic not valid");
        }
        if(PhysicsSimulator::get()!=nullptr){
            logger->debug("update PhysicsSimulator");
            PhysicsSimulator::get()->update(deltaTime);
            
        }
        if(ActorManager::get()!=nullptr){
            logger->debug("update ActorManager");
            ActorManager::get()->update(deltaTime);
        } else{
            logger->info("ActorManager not valid");
        }

        // generate output
        if(Graphics::get()!=nullptr){
            logger->debug("update GraphicsEngine");
            Graphics::get()->update(deltaTime);
        } else{
            logger->info("GraphicsEngine not valid");
        }
    }
    logger->debug("game update ended");
}