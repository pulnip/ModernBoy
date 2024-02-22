#pragma once

#include "Engine/InputSystem.hpp"

namespace WithSDL{
    class InputSystem final: public Engine::InputSystem{
        friend class Core;
      public:
        InputSystem() noexcept;
        ~InputSystem();

      private:
        void update(const Game::Time& deltaTime) noexcept override;

        uint8_t escapeKeycode() noexcept override final;

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}

