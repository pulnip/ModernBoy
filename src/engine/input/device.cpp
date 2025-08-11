#include <print>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include "engine/input/device.hpp"
#include "engine/interface/input_service.hpp"

using namespace std::chrono_literals;
using namespace ModernBoy;
using namespace ModernBoy::Input;
using namespace ModernBoy::Interface;

Device::Device(SDL_Window* window, InputService& inputSrv)
:ema(0ms), window(window)
,sdlKeyboard(SDL_GetKeyboardState(nullptr))
,service(inputSrv){
    SDL_GetWindowSize(window, &width, &height);
}

TaskTime Device::expectedExecTime(){
    return ema;
}

void Device::update(DeltaTime){
    auto started = std::chrono::steady_clock::now();

    State statePrev = service.snapshot();
    State stateCurr;

    for(int i=0; i<KeyCode::KEY_UNKNOWN; ++i){
        auto key = static_cast<KeyCode>(i);
        auto sdlCode = convert(key);

        uint8_t active = sdlKeyboard[sdlCode] ? ACTIVE_FLAG : 0;

        auto newState = transit(statePrev.keyboard[key], active);
        stateCurr.keyboard[key] = newState;
    }

    float x, y, dx, dy;
    SDL_GetMouseState(&x, &y);
    SDL_GetRelativeMouseState(&dx, &dy);
    stateCurr.mouse = Mouse{
        .pos0 = statePrev.mouse.pos,
        .pos = Vec2{
            .x = 2*x/height - float(width)/height,
            .y = 1 - 2*y/height,
        },
        .dpos = Vec2{
            .x = 2*dx/height,
            .y = 2*dy/height
        }
    };

    service.write(stateCurr);

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

void Device::updateEMA(TaskTime elapsed){
    auto alpha = 1.0 / policy.effective_window_size;
    long long blended = (1-alpha)*ema.count() + alpha*elapsed.count();
    ema = TaskTime(blended);
}
