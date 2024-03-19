#pragma once

#include <vector>

#include "Blueprint.hpp"
#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Actors{

}

namespace Engine{
    class ActorManager:
        public MakableSingleton<ActorManager, false, false>
    {
        friend class ::MainEngine;

      public:
        ActorManager() noexcept=default;
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