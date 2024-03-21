#include "Engine/GameLogic.hpp"
#include "Engine/SystemLocator.hpp"

void GameLogic::update(const Time& deltaTime) noexcept{
    // nothing...
}

void GameLogic::setStatus(const Status& status) noexcept{
    switch(status){
      case Game::Status::FORCE_QUIT:
        [[fallthrough]];
      case Game::Status::GAME_OVER:
        [[fallthrough]];
      case Game::Status::UNEXPECTED:
        Getter::core().stop();
    }
}