#include "input/chord.hpp"

using namespace std::chrono_literals;
using namespace ModernBoy;
using namespace ModernBoy::Input;

Chord::Chord(SDL_Window* window):device(window), ema(0ms){}

TaskTime Chord::expectedExecTime(){
    return ema;
}

void Chord::update(DeltaTime){
    auto started = std::chrono::steady_clock::now();

    device.fetch(state[1 - readIndex]);
    swapState();

    auto elapsed = std::chrono::steady_clock::now() - started;
    updateEMA(std::chrono::duration_cast<TaskTime>(elapsed));
}

// Generator<void> Chord::update(DeltaTime){
//     auto started = std::chrono::steady_clock::now();

//     device.fetch(state[1 - readIndex]);
//     swapState();

//     auto elapsed = std::chrono::steady_clock::now() - started;
//     updateEMA(std::chrono::duration_cast<TaskTime>(elapsed));
//     co_return;
// }

bool Chord::query(KeyCode keyCode, KeyState keyState
){
    return state[readIndex].keyboard[keyCode] == keyState;
}

Vec2 Chord::mouse(){
    return {.x = state->mouse.dx, .y = state->mouse.dy};
}

void Chord::updateEMA(TaskTime elapsed){
    auto alpha = 1.0 / policy.effective_window_size;
    long long blended = (1-alpha)*ema.count() + alpha*elapsed.count();
    ema = TaskTime(blended);
}

void Chord::swapState(){
    readIndex.store(
        1 - readIndex.load(
            std::memory_order_acquire),
    std::memory_order_release);
}
