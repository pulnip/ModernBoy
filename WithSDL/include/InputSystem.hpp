#pragma once

#include "Engine/InputSystem.hpp"
#include "Engine/Logger.hpp"

namespace WithSDL{
    class InputSystem final: public Engine::InputSystem{
        friend class Core;
      public:
        static void make() noexcept{
            ::Engine::InputSystem::make<InputSystem>();
        }

      private:
        void update(const Game::Time& deltaTime) noexcept override;

        uint8_t escapeKeycode() noexcept override final;

      private:
        ::Logger::Binded logger={"Input", "SDL"};
        uint8_t oldKeyState[sizeof(uint8_t)]={0, };
    };
}

