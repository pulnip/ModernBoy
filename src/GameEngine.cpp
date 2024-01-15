#include <algorithm>

#include <SDL2/SDL_timer.h>

#include "GameEngine/GameEngine.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/InputSystem.hpp"
#include "Timer.hpp"

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