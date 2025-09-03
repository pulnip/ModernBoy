#ifndef MODERNBOY_INTERFACE_INPUTSERVICE_HPP
#define MODERNBOY_INTERFACE_INPUTSERVICE_HPP

#include "input/state.hpp"

namespace ModernBoy::Interface
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

#endif // MODERNBOY_INTERFACE_INPUTSERVICE_HPP
