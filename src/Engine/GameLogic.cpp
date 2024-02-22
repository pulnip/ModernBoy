#include "Engine/Core.hpp"
#include "Engine/GameLogic.hpp"
#include "Engine/Logger.hpp"

using namespace Engine;

GameLogic::GameLogic() noexcept:
    logger(std::make_unique<BindedLogger>("GameLogic", "Base"))
{
    logger->debug("constructed");
}

GameLogic::~GameLogic(){
    logger->debug("destructed");
}

void GameLogic::connect() noexcept{
    subscribe(Core::get());
}

void GameLogic::update(const Game::Time& deltaTime) noexcept{
    logger->debug("update");
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