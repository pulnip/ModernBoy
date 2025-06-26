#include <utility>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include "input/state.hpp"
#include "input/device.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

Device::Device()
:sdl_key(SDL_GetKeyboardState(nullptr)){
    keyState.fill(ButtonState::None);
}
Device::~Device(){}
Device::Device(Device&& other){
    moveFrom(std::move(other)); }
Device& Device::operator=(Device&& other){
    moveFrom(std::move(other));
    return *this;
}
void Device::moveFrom(Device&& other){
    sdl_key = other.sdl_key;
    other.sdl_key = nullptr;
}

void Device::fetch(State& state){
    // SDL_PumpEvents();

    for(int i=0; i<Button::KEY_UNKNOWN; ++i){
        auto code = static_cast<Button>(i);
        auto sdl_code = convert(code);
        uint8_t active = sdl_key[sdl_code] ? ACTIVE_FLAG : 0;

        auto newState = transit(keyState[code], active);
        keyState[code] = newState;
    }
}
