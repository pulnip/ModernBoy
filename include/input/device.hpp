#ifndef MODERNBOY_INPUT_DEVICE_HPP
#define MODERNBOY_INPUT_DEVICE_HPP

#include "fwd.hpp"

namespace ModernBoy::Input
{
    struct State;

    class Device{
    public:
        Device(SDL_Window* window);
        ~Device() = default;
        Device(const Device& other)=delete;
        Device(Device&&);
        Device& operator=(const Device& other)=delete;
        Device& operator=(Device&&);

        void fetch(State& state);

    private:
        void moveFrom(Device&& other);

        SDL_Window* window;
        const bool* sdlKeyboard = nullptr;
    };
}

#endif // MODERNBOY_INPUT_DEVICE_HPP