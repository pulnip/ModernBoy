#ifndef MODERNBOY_INPUT_CHORD_HPP
#define MODERNBOY_INPUT_CHORD_HPP

#include <atomic>
#include <SDL3/SDL_video.h>
#include "interface.hpp"
#include "input/device.hpp"
#include "input/state.hpp"

namespace ModernBoy::Input
{
    class Chord{
    public:
        Chord(SDL_Window* window);

        TaskTime expectedExecTime();
        void update(DeltaTime);
        // Generator<void> update(DeltaTime deltatime);

        bool query(KeyCode, KeyState);

        Vec2 mousePos();
        Vec2 mouseMove();

        const TaskPolicy policy{
            .effective_window_size = 10,
            .min_interval = std::chrono::microseconds(100),
            .patience = std::chrono::milliseconds(200)
        };

    private:
        void updateEMA(TaskTime);
        void swapState();

        State state[2];
        std::atomic<int> readIndex = 0;
        Device device;

        // exponential moving average
        TaskTime ema;
    }; static_assert(Subsystem<Chord>);
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_CHORD_HPP