#include <print>
#include <utility>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include "input/state.hpp"
#include "input/device.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

Device::Device(SDL_Window* window)
:window(window), sdlKeyboard(SDL_GetKeyboardState(nullptr)){

    SDL_SetWindowRelativeMouseMode(window, true);
}
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

    // if(state.keyboard[KEY_SHIFT] == Pressed){
    //     SDL_SetWindowRelativeMouseMode(window, false);
    // }
    // else if(state.keyboard[KEY_SHIFT] == Released){
    //     SDL_SetWindowRelativeMouseMode(window, true);
    // }

    // auto x0 = state.mouse.x, y0 = state.mouse.y;
    SDL_GetRelativeMouseState(&state.mouse.dx, &state.mouse.dy);
    // state.mouse.dx = state.mouse.x - x0;
    // state.mouse.dy = state.mouse.y - y0;

    // state.mouse.dx = 0;
    // state.mouse.dy = 0;

    // SDL_Event event;
    // if(!SDL_PollEvent(&event))
    //     return;

    // if(event.type == SDL_EVENT_MOUSE_MOTION){
    //     state.mouse = {
    //         .x = event.motion.x,
    //         .y = event.motion.y,
    //         .dx = event.motion.xrel,
    //         .dy = event.motion.yrel
    //     };
    // }
}
