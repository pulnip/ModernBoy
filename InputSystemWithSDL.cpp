#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>

#include "InputSystemWithSDL.hpp"
#include "PubSubMessage.hpp"

void InputSystemWithSDL::update(const float &deltaTime) noexcept {
    SDL_Event event;
    // 큐에 여전히 이벤트가 남아있는 동안
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            Observable<GameStatus>::notify(GameStatus::FORCE_QUIT);
            break;
        }
    }

    const uint8_t *keyState = SDL_GetKeyboardState(nullptr);
    // ESC로 게임 종료
    if (keyState[SDL_SCANCODE_ESCAPE]) {
        Observable<GameStatus>::notify(GameStatus::FORCE_QUIT);
    }
}

void InputSystemWithSDL::postConstruct() noexcept {
#warning "Not defined"
}
