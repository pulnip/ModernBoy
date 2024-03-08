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

// std::string Engine::title;
// Blueprint::Screen Engine::screen;

Core::Core(std::unique_ptr<Timer> timer) noexcept:
    timer(std::move(timer))
{
    if(this->timer==nullptr){
        logger.error("timer not valid");
    }
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
            GameLogic::get()->update(deltaTime);
        } else{
            logger.info("GameLogic not valid");
        }
        if(PhysicsSimulator::get()!=nullptr){
            PhysicsSimulator::get()->update(deltaTime);            
        }
        if(ActorManager::get()!=nullptr){
            ActorManager::get()->update(deltaTime);
        } else{
            logger.info("ActorManager not valid");
        }

        // generate output
        if(Graphics::get()!=nullptr){
            Graphics::get()->update(deltaTime);
        } else{
            logger.info("GraphicsEngine not valid");
        }
    }
}