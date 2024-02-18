#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "SubEngine.hpp"

namespace Game{
    namespace SubEngine{
        class ActorManager: public Interface{
        public:
            ActorManager() noexcept=default;
            virtual ~ActorManager()=default;

            Type getType() const noexcept override final{
                return Type::ActorManager;
            }
            std::optional<std::shared_ptr<Interface>> query(
            Type type) noexcept;

        private:
            void update(const Time& deltaTime) noexcept override final;

            void appendActor(
            std::shared_ptr<Actor::Interface> actor) noexcept;
            void removeActor(
            std::shared_ptr<Actor::Interface> actor) noexcept;

        private:
            bool isUpdatingActors = false;
            std::vector<std::shared_ptr<Actor::Interface>> actors;
            std::vector<std::shared_ptr<Actor::Interface>> pendingActors;
        };
    }
}

namespace Null{
    namespace SubEngine{
        class NullActorManager final:
            public Game::SubEngine::ActorManager{};
    }
}