#include <SDL2/SDL_keyboard.h>
#include "InputComponent.hpp"
#include "SDL_InputSystem.hpp"

using namespace std;
using namespace ModernBoy;

SDL_KeyboardState::SDL_KeyboardState()
: current(SDL_GetKeyboardState(nullptr)){}

void SDL_KeyboardState::saveCurrentState(){
    memcpy(previous, current, SDL_NUM_SCANCODES);
}

SDL_InputSystem::SDL_InputSystem()=default;
SDL_InputSystem::~SDL_InputSystem()=default;

void SDL_InputSystem::prepare(){
    kbState.saveCurrentState();
}

void SDL_InputSystem::notify(const EventInput& input){
    std::lock_guard lg(mtx);

    InputState inputState{
        .kbRef={
            .current=kbState.current,
            .previous=kbState.previous
        },
        .btn=input.btn
    };

    for(const auto& w: observers){
        if(not w.expired()){
            w.lock()->onPressed(inputState);
        }
    }
}

void SDL_InputSystem::notifyReleased(const EventInput& input){
    std::lock_guard lg(mtx);

    InputState inputState{
        .kbRef={
            .current=kbState.current,
            .previous=kbState.previous
        },
        .btn=input.btn
    };

    for(const auto& w: observers){
        if(not w.expired()){
            w.lock()->onReleased(inputState);
        }
    }
}
