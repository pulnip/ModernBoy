#ifndef MODERNBOY_INPUT_CHORD_HPP
#define MODERNBOY_INPUT_CHORD_HPP

#include <atomic>
#include <SDL3/SDL_video.h>
#include "engine/interface.hpp"
#include "engine/input/state.hpp"

namespace ModernBoy::Input
{
    class Device{
    public:
        Device(SDL_Window*, Interface::InputService&);

        TaskTime expectedExecTime();
        void update(DeltaTime);
        // Generator<void> update(DeltaTime deltatime);

        const TaskPolicy policy{
            .effective_window_size = 10,
            .min_interval = std::chrono::microseconds(100),
            .patience = std::chrono::milliseconds(200)
        };

    private:
        void updateEMA(TaskTime);

        // for normalize mouse coord
        int width, height;

        // exponential moving average
        TaskTime ema;

        SDL_Window* window;
        const bool* sdlKeyboard = nullptr;

        Interface::InputService& service;
    }; static_assert(Subsystem<Device>);
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_CHORD_HPP