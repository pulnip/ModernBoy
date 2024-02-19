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
            public Receiver,
            public std::enable_shared_from_this<Engine>
        {
        public:
            Engine(std::shared_ptr<Game::SubEngine::Logger>& logger
            ) noexcept: logger(logger){}
            virtual ~Engine() = default;

            void postConstruct() noexcept override;
            void run() noexcept;
            void setLogger(std::shared_ptr<SubEngine::Logger> logger
            ) noexcept{ this->logger=logger; }
            void setTimer(std::shared_ptr<SubEngine::Timer> timer
            ) noexcept{ this->timer=timer; }
            void setResourceManager(
                std::shared_ptr<SubEngine::ResourceManager> rm
            ) noexcept{ resourceManager=rm; }
            void setInputSystem(
                std::shared_ptr<SubEngine::InputSystem> is
            ) noexcept{ inputSystem=is; }
            void setGameLogic(
                std::shared_ptr<SubEngine::GameLogic>& gl
            ) noexcept{ gameLogic=gl; }
            void setPhysicsSimulator(
                std::shared_ptr<SubEngine::PhysicsSimulator> ps
            ) noexcept{ physicsSimulator=ps; }
            void setActorManager(
                std::shared_ptr<SubEngine::ActorManager> am
            ) noexcept{ actorManager=am; }
            void setGraphicsEngine(
                std::shared_ptr<SubEngine::GraphicsEngine> ge
            ) noexcept{ graphicsEngine=ge; }
            void setSoundEngine(
                std::shared_ptr<SubEngine::SoundEngine> se
            ) noexcept{ soundEngine=se; }
            Plugin find(const SubEngine::Type name) noexcept;

        private:
            void handler() noexcept override final;

        protected:
            std::shared_ptr<SubEngine::Logger> logger;
            std::shared_ptr<SubEngine::Timer> timer;
            std::shared_ptr<SubEngine::ResourceManager> resourceManager;

            std::shared_ptr<SubEngine::InputSystem> inputSystem;
            std::shared_ptr<SubEngine::GameLogic> gameLogic;
            std::shared_ptr<SubEngine::PhysicsSimulator> physicsSimulator;
            std::shared_ptr<SubEngine::ActorManager> actorManager;
            std::shared_ptr<SubEngine::GraphicsEngine> graphicsEngine;
            std::shared_ptr<SubEngine::SoundEngine> soundEngine;

        private:
            bool isRunning=false;
        };
    }
}
