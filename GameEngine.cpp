#include <algorithm>

#include <SDL2/SDL_timer.h>

#include "ActorManager.hpp"
#include "GameEngine.hpp"
#include "GameLogic.hpp"
#include "GraphicsEngine.hpp"
#include "InputSystem.hpp"

void GameEngine::run() noexcept {
    static uint32_t ticksCount = 0;
    // 마지막 프레임 이후로 최소한 16ms가 경과할 때까지 대기
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksCount + 16))
        ;

    // 델타 시간은 현재 프레임의 틱 값과 이전 프레임 틱 값의 차다.
    float deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0f; // ms -> s
    // 최대 델타 시간 값은 0.05s
    deltaTime = std::min(0.05f, deltaTime);
    // 다음 프레임을 위해 틱 값을 갱신
    ticksCount = SDL_GetTicks();

    while (gameLogic->isReady()) {
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