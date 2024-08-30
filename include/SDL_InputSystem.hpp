#pragma once

#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_mouse.h>
#include "InputSystem.hpp"

namespace ModernBoy{
    struct SDL_KeyboardState{
        const uint8_t* current;
        uint8_t previous[SDL_NUM_SCANCODES]={0, };

        SDL_KeyboardState();
        void saveCurrentState();
    };

    class SDL_InputSystem final: public InputSystem{
        // SDL_InputState
        SDL_KeyboardState kbState{};

      public:
        SDL_InputSystem();
        ~SDL_InputSystem();

        void prepare() override;
        void notify(const EventInput& input) override;
        void notifyReleased(const EventInput& input) override;
    };
}
