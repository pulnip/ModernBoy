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
        public MakableSingleton<ActorManager>
    {
        friend class ::MainEngine;

      public:
        static void preConstruct() noexcept{}
        ActorManager() noexcept=default;
        virtual void postConstruct() noexcept override final{}
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