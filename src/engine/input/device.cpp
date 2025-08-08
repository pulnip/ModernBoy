#include <print>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include "engine/input/device.hpp"

using namespace std::chrono_literals;
using namespace ModernBoy;
using namespace ModernBoy::Input;

Device::Device(SDL_Window* window)
:ema(0ms), window(window),
sdlKeyboard(SDL_GetKeyboardState(nullptr)){
    SDL_GetWindowSize(window, &width, &height);
}

TaskTime Device::expectedExecTime(){
    return ema;
}

void Device::update(DeltaTime){
    auto started = std::chrono::steady_clock::now();

    State& stateWrite = state[1 - readIndex];
    State& stateRead = state[readIndex];

    for(int i=0; i<KeyCode::KEY_UNKNOWN; ++i){
        auto key = static_cast<KeyCode>(i);
        auto sdlCode = convert(key);

        uint8_t active = sdlKeyboard[sdlCode] ? ACTIVE_FLAG : 0;

        auto newState = transit(stateRead.keyboard[key], active);
        stateWrite.keyboard[key] = newState;
    }

    SDL_GetMouseState(&stateWrite.mouse.x, &stateWrite.mouse.y);
    SDL_GetRelativeMouseState(&stateWrite.mouse.dx, &stateWrite.mouse.dy);


    swapState();

    auto elapsed = std::chrono::steady_clock::now() - started;
    updateEMA(std::chrono::duration_cast<TaskTime>(elapsed));
}

// Generator<void> Device::update(DeltaTime){
//     auto started = std::chrono::steady_clock::now();

//     device.fetch(state[1 - readIndex]);
//     swapState();

//     auto elapsed = std::chrono::steady_clock::now() - started;
//     updateEMA(std::chrono::duration_cast<TaskTime>(elapsed));
//     co_return;
// }

bool Device::query(KeyCode keyCode, KeyState keyState
){
    return state[readIndex].keyboard[keyCode] == keyState;
}

Vec2 Device::mouseMove(){
    return {.x = state->mouse.dx, .y = state->mouse.dy};
}
Vec2 Device::mousePos(){
    return {.x = 2*state->mouse.x/height - float(width)/height, .y = 1 - 2*state->mouse.y/height};
}

void Device::updateEMA(TaskTime elapsed){
    auto alpha = 1.0 / policy.effective_window_size;
    long long blended = (1-alpha)*ema.count() + alpha*elapsed.count();
    ema = TaskTime(blended);
}

void Device::swapState(){
    readIndex.store(
        1 - readIndex.load(
            std::memory_order_acquire),
    std::memory_order_release);
}
