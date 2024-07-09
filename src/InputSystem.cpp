#include <utility>
#include <SDL2/SDL_keyboard.h>
#include "InputSystem.hpp"

using namespace std;
using namespace ModernBoy;

KeyboardState::KeyboardState()
:current(SDL_GetKeyboardState(nullptr)){}

void KeyboardState::update(){
    memcpy(previous, current, SDL_NUM_SCANCODES);
}

bool KeyboardState::isPressed(SDL_Scancode key) const{
    return current[key];
}

ButtonState KeyboardState::getKey(SDL_Scancode key) const{
    using enum ButtonState;

    const auto pressed = previous[key]!=0;
    const auto pressing = current[key]!=0;

    const auto press = (static_cast<int>(pressed)<<1) + static_cast<int>(pressing);

    switch(press){
    case 0b00:
        return NONE;
    case 0b01:
        return PRESSED;
    case 0b10:
        return RELEASED;
    case 0b11:
        return HELD;
    default:
        unreachable();
    }
}

void InputSystem::update(){
    input.keyboard.update();
}

const InputState& InputSystem::getInput() const noexcept{
    return input;
}
