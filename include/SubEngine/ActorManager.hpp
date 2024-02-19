#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "SubEngine.hpp"

namespace Game{
    namespace SubEngine{
        class ActorManager: public Interface,
            public std::enable_shared_from_this<ActorManager>
        {
        public:
            ActorManager() noexcept=default;
            virtual ~ActorManager()=default;

            void postConstruct() noexcept override;
            void update(const Time& deltaTime) noexcept override final;
            Type getType() const noexcept override final{
                return Type::ActorManager;
            }
            std::optional<Plugin> query(Type type) noexcept;
            void appendActor(Object actor) noexcept;
            void removeActor(Object actor) noexcept;

        private:
            bool isUpdatingActors = false;
            std::vector<Object> actors;
            std::vector<Object> pendingActors;
        };
    }
}

namespace Null{
    namespace SubEngine{
        class NullActorManager final:
            public Game::SubEngine::ActorManager{};
    }
}