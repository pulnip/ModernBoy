#pragma once

#include <vector>

#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class ActorManager:
        public Singleton<ActorManager>
    {
        friend class ::MainEngine;

      public:
        virtual ~ActorManager()=default;

        void update(const Game::Time& deltaTime) noexcept;

        void appendActor(pActor actor) noexcept;
        void removeActor(pActor actor) noexcept;

      private:
        ::Logger::Binded logger={"ActorManager", "base"};
        bool isUpdatingActors = false;
        std::vector<pActor> actors;
        std::vector<pActor> pendingActors;
    };
}