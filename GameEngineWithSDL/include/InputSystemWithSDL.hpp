#pragma once

#include "SubEngine/InputSystem.hpp"

namespace WithSDL{
    namespace SubEngine{
        class InputSystem final: public Game::SubEngine::InputSystem{
        public:
            InputSystem() noexcept=default;
            ~InputSystem()=default;

        private:
            void update(const Game::Time& deltaTime) noexcept override;

            uint8_t escapeKeycode() const noexcept override final;
        };

    }
}

