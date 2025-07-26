#ifndef MODERNBOY_INPUT_CHORD_HPP
#define MODERNBOY_INPUT_CHORD_HPP

#include <atomic>
#include "interface.hpp"
#include "input/device.hpp"
#include "input/state.hpp"

namespace ModernBoy::Input
{
    class Chord{
    public:
        Chord();

        TaskTime expectedExecTime();
        void update(DeltaTime);
        // Generator<void> update(DeltaTime deltatime);

        bool query(KeyCode, KeyState);

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