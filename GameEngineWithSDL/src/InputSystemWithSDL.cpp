#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>

#include "Skin.hpp"
#include "GameEngine/GameEngine.hpp"
#include "InputSystemWithSDL.hpp"
#include "SubEngine/GameLogic.hpp"

using namespace Game;
using namespace WithSDL::SubEngine;

void InputSystem::update(const Time& deltaTime) noexcept {
    SDL_Event event;
    // 큐에 여전히 이벤트가 남아있는 동안
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            Observable<Status>::notify(Status::FORCE_QUIT);
        }
    }
    
    const uint8_t *keyState = SDL_GetKeyboardState(nullptr);

    if(keyState[escapeKeycode()]){
        Observable<Status>::notify(Status::GAME_OVER);
    }

    for(auto& [code, target]: keyMap){
        if(keyState[code]){
            ::Skin::Key key{::Skin::Key::Status::PRESSED, code};
            target.notify(key);
        }
    }
}

uint8_t InputSystem::escapeKeycode() const noexcept{
    return SDL_SCANCODE_ESCAPE;
}
