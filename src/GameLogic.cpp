#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/InputSystem.hpp"

using namespace Game;
using namespace Game::SubEngine;

void GameLogic::postConstruct() noexcept{
    assert(not owner.expired());
    auto self=shared_from_this();
    owner.lock()->setGameLogic(self);

    UniqueSender::subscribe(owner);
}

void GameLogic::onNotify(const Status& status) noexcept{
    switch(status){
    case Status::FORCE_QUIT:
        [[fallthrough]];
    case Status::GAME_OVER:
        [[fallthrough]];
    case Status::UNEXPECTED:
        UniqueSender::notify();
    }
}
