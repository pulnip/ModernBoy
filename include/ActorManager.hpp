#pragma once

#include <memory>
#include <vector>
#include "fwd.hpp"

namespace ModernBoy{
    class ActorManager{
        std::vector<std::shared_ptr<Actor>> actors;

      public:
        ActorManager();
        void update(float dt);

        void addActor(std::shared_ptr<Actor> actor);
    };
}
