#include <utility>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include "input/device.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

Device::Device()
:sdl_keystate(SDL_GetKeyboardState(nullptr)){}
Device::~Device(){}
Device::Device(Device&& other){
    moveFrom(std::move(other)); }
Device& Device::operator=(Device&& other){
    moveFrom(std::move(other));
    return *this;
}
void Device::moveFrom(Device&& other){
    sdl_keystate = other.sdl_keystate;
    other.sdl_keystate = nullptr;
}

void Device::fetch(State& state){
    SDL_PumpEvents();

    for(int i=0; i<Button::KEY_UNKNOWN; ++i){
        auto code = static_cast<Button>(i);
        auto sdl_code = convert(code);
        uint8_t active = sdl_keystate[sdl_code] ? ACTIVE_FLAG : 0;

        auto newState = transit(state.key[code], active);
        state.key[code] = newState;
    }
}
