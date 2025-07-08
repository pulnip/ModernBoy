#include <utility>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include "input/state.hpp"
#include "input/device.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

Device::Device()
:sdlKeyboard(SDL_GetKeyboardState(nullptr)){}
Device::~Device(){}
Device::Device(Device&& other){
    moveFrom(std::move(other)); }
Device& Device::operator=(Device&& other){
    moveFrom(std::move(other));
    return *this;
}
void Device::moveFrom(Device&& other){
    sdlKeyboard = other.sdlKeyboard;
    other.sdlKeyboard = nullptr;
}

void Device::fetch(State& state){
    // SDL_PumpEvents();

    for(int i=0; i<KeyCode::KEY_UNKNOWN; ++i){
        auto key = static_cast<KeyCode>(i);
        auto sdlCode = convert(key);
        uint8_t active = sdlKeyboard[sdlCode] ? ACTIVE_FLAG : 0;

        auto newState = transit(state.keyboard[key], active);
        state.keyboard[key] = newState;
    }
}
