#pragma once

#include "Engine/InputSystem.hpp"
#include "Engine/Logger.hpp"

namespace WithSDL{
    class InputSystem final: public Engine::InputSystem{
        friend class Core;
      private:
        void update(const Game::Time& deltaTime) noexcept override;

        uint8_t escapeKeycode() noexcept override final;

      private:
        ::Logger::Binded logger={"Input", "SDL"};
    };
}

