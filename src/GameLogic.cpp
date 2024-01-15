#include <SDL2/SDL_log.h>

#include "PubSubMessage.hpp"
#include "SubEngine/GameLogic.hpp"

void GameLogic::update(const float &deltaTime) noexcept {
#warning "Not defined"
}

void GameLogic::postConstruct() noexcept {
#warning "Not defined"
}

void GameLogic::onNotify(GameStatus status) noexcept {
    switch (status) {
    case GameStatus::GAME_OVER:
        SDL_Log("Game Over");
        break;
    case GameStatus::FORCE_QUIT:
        SDL_Log("Force Quit");
        break;
    case GameStatus::UNEXPECTED:
        SDL_Log("Unexpected");
        break;
    default:
        SDL_Log("wtf");
    }
    ready = false;
#warning "Not defined"
}
