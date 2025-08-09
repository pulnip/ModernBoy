#ifndef MODERNBOY_SERVICE_INPUTSERVICE_HPP
#define MODERNBOY_SERVICE_INPUTSERVICE_HPP

#include "engine/input/state.hpp"

namespace ModernBoy::Service
{
    class InputService{
    public:
        InputService() = default;
        ~InputService() = default;

        void write(const Input::State& state);

        const Input::State& snapshot() const;

    private:
        Input::State state;
    };
}

#endif // MODERNBOY_SERVICE_INPUTSERVICE_HPP
