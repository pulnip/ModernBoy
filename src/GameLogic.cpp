#include <cassert>

#include "PubSubMessage.hpp"
#include "GameEngine/GameEngine.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/InputSystem.hpp"

void GameLogic::injectDependency() noexcept{
    assert(!owner.expired());
    auto ge=std::dynamic_pointer_cast<GameEngine>(
        owner.lock()
    );

    assert(ge!=nullptr);
    UniqueSender::subscribe(ge);
}

void GameLogic::onNotify(GameStatus status) noexcept{
    switch(status){
    case GameStatus::FORCE_QUIT:
        [[fallthrough]];
    case GameStatus::GAME_OVER:
        [[fallthrough]];
    case GameStatus::UNEXPECTED:
        UniqueSender::notify();
    }
}
