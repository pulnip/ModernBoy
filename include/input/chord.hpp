#ifndef MODERNBOY_INPUT_CHORD_HPP
#define MODERNBOY_INPUT_CHORD_HPP

#include "input/device.hpp"
#include "input/state.hpp"

namespace ModernBoy::Input
{
    class Chord{
    public:
        bool query(KeyCode, KeyState);

    public:
        State state;
        Device device;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_CHORD_HPP