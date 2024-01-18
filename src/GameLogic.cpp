#include <cassert>
#include <SDL2/SDL_log.h>

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
    Observable<GameStatus>::subscribe(ge);
}

void GameLogic::onNotify(Key key) noexcept{
    this->Observable<GameStatus>::notify(GameStatus::FORCE_QUIT);
}
