#pragma once

#include <SDL2/SDL_scancode.h>

namespace ModernBoy{
    enum class ButtonState{
        NONE,
        PRESSED,
        RELEASED,
        HELD
    };

    class KeyboardState{
        const uint8_t* const current;
        uint8_t previous[SDL_NUM_SCANCODES] = {0, };

      public:
        KeyboardState();

        void update();

        bool isPressed(SDL_Scancode key) const;
        ButtonState getKey(SDL_Scancode key) const;
    };

    struct InputState{
        KeyboardState keyboard;
    };

    class InputSystem{
        InputState input;

      public:
        // call after SDL_PollEvent
        void update();

        const InputState& getInput() const noexcept;
    };
}
