#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>

#include "PubSubMessage.hpp"
#include "InputSystemWithSDL.hpp"

void InputSystemWithSDL::update(const float &deltaTime) noexcept {
    SDL_Event event;
    // 큐에 여전히 이벤트가 남아있는 동안
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            keyMap[SDL_SCANCODE_ESCAPE].notify(
                {Key::Status::PRESSED, SDL_SCANCODE_ESCAPE}
            );
            break;
        }
    }
    
    const uint8_t *keyState = SDL_GetKeyboardState(nullptr);

    for(auto& [key, target]: keyMap){
        if (keyState[key]) {
            target.notify( {Key::Status::PRESSED, key} );
        }
    }
}

void InputSystemWithSDL::injectDependency() noexcept {
    InputSystem::injectDependency();
}

uint8_t InputSystemWithSDL::escapeKeycode() const noexcept{
    return SDL_SCANCODE_ESCAPE;
}
