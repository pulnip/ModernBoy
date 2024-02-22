#pragma once

#include "Component/Ability.hpp"

namespace Component{
    class AI: public Ability{
      public:
        enum class State {
            Death,
            Patrol,
            Attack
        };

      public:
        AI(std::weak_ptr<Actor::Vanilla> actor) noexcept;
        virtual ~AI();
    
      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}