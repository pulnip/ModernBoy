#pragma once

#include <cstdint>

namespace ModernBoy{
    struct KeyboardState{
        const uint8_t* const& current;
        const uint8_t* const& previous;

        bool isPressed(uint8_t kbKey) const noexcept;
    };

    struct ButtonState{
        int32_t keyType;
        bool keydown;
        bool repeat;
    };
}
