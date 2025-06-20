#ifndef MODERNBOY_INPUT_DEVICE_HPP
#define MODERNBOY_INPUT_DEVICE_HPP

#include "input/state.hpp"

namespace ModernBoy{ namespace Input{
    class Device{
    public:
        Device();
        ~Device();
        Device(const Device& other)=delete;
        Device(Device&&);
        Device& operator=(const Device& other)=delete;
        Device& operator=(Device&&);

        State fetch();

    private:
        void moveFrom(Device&& other);
    };
}}

#endif // MODERNBOY_INPUT_DEVICE_HPP