#include "Engine/Core.hpp"
#include "Engine/GameLogic.hpp"
#include "Engine/Logger.hpp"

using namespace Engine;

void GameLogic::connect() noexcept{
    subscribe(Core::get());
}

void GameLogic::update(const Game::Time& deltaTime) noexcept{
    // nothing...
}

void GameLogic::onNotify(const Game::Status& status) noexcept{
    switch(status){
      case Game::Status::FORCE_QUIT:
        [[fallthrough]];
      case Game::Status::GAME_OVER:
        [[fallthrough]];
      case Game::Status::UNEXPECTED:
        UniqueSender::notify();
    }
}