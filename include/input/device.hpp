#ifndef MODERNBOY_INPUT_DEVICE_HPP
#define MODERNBOY_INPUT_DEVICE_HPP

#include "input/state.hpp"
#include "fwd.hpp"

namespace ModernBoy::Input
{
    class Device{
    public:
        Device();
        ~Device();
        Device(const Device& other)=delete;
        Device(Device&&);
        Device& operator=(const Device& other)=delete;
        Device& operator=(Device&&);

        void fetch(State& state);

    private:
        void moveFrom(Device&& other);

        State state;
        const bool* sdl_keystate = nullptr;
    };
}

#endif // MODERNBOY_INPUT_DEVICE_HPP