#pragma once

#include "myfwd.hpp"
#include "Engine/Logger.hpp"
#include "Actor/Vanilla.hpp"

namespace Test{
    class Wall: public Actor::Vanilla{
      public:
        virtual void initAbility() noexcept override=0;

      private:
        void updateActor(const Game::Time&) noexcept override final{}

      private:
        Logging::Bind logger={"Wall", id};
    };

    class Ceil final: public Wall{
      public:
        void initAbility() noexcept override final;
    };

    class Floor final: public Wall{
      public:
        void initAbility() noexcept override final;
    };

    class RightWall final: public Wall{
      public:
        void initAbility() noexcept override final;
    };
}
