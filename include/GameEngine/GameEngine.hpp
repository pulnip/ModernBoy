#pragma once

#include <map>
#include <memory>
#include <functional>

#include "Makable.hpp"
#include "Observer.hpp"

#include "gefwd.hpp"

enum class EngineCommand{
    QUIT
};

namespace Game{
    namespace Core{
        class Engine: public Makable<Engine, MainEngine>,
            public Receiver
        {
        public:
            Engine(std::shared_ptr<Game::SubEngine::Logger>& logger
            ) noexcept: logger(logger){}
            virtual ~Engine() = default;

            void run() noexcept;
            std::shared_ptr<SubEngine::Interface> find(
            const SubEngine::Type name) noexcept;

        private:
            void handler() noexcept override final;

        private:
            bool isRunning=false;

            std::shared_ptr<SubEngine::Logger> logger;
            std::shared_ptr<SubEngine::Timer> timer;
            std::shared_ptr<SubEngine::ResourceManager> resourceManager;

            std::shared_ptr<SubEngine::InputSystem> inputSystem;
            std::shared_ptr<SubEngine::GameLogic> gameLogic;
            std::shared_ptr<SubEngine::PhysicsSimulator> physicsSimulator;
            std::shared_ptr<SubEngine::ActorManager> actorManager;
            std::shared_ptr<SubEngine::GraphicsEngine> graphicsEngine;
            std::shared_ptr<SubEngine::SoundEngine> soundEngine;
        };
    }
}
