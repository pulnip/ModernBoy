#pragma once

#include <vector>

#include "TinyTraits.hpp"
#include "myfwd.hpp"

namespace Engine{
    class ActorManager: public Singleton<ActorManager>{
        friend class ::MainEngine;

      public:
        ActorManager() noexcept;
        virtual ~ActorManager();

        void update(const Game::Time& deltaTime) noexcept;

        void appendActor(pActor actor) noexcept;
        void removeActor(pActor actor) noexcept;

      private:
        std::unique_ptr<BindedLogger> logger;

        bool isUpdatingActors = false;
        std::vector<pActor> actors;
        std::vector<pActor> pendingActors;
    };
}