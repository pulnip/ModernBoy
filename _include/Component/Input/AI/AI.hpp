#pragma once

#include "Component/Ability.hpp"
#include "Engine/Logger.hpp"

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
        virtual ~AI()=default;
    
      private:
        ::Logger::Binded logger={"AI", id};
    };
}