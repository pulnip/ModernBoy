#pragma once

#include <memory>

#include "gefwd.hpp"
#include "GameEngine/GameEngine.hpp"

using namespace Game;

namespace WithSDL{
    namespace Core{
        class Engine: public Game::Core::Engine{
        public:
            Engine(std::shared_ptr<Game::SubEngine::Logger>& logger) noexcept;
            virtual ~Engine();

        protected:
            void postConstruct() noexcept override;
        };
    }
}