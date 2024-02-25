#pragma once

#include "myfwd.hpp"
#include "Actor/Vanilla.hpp"

namespace Test{
    class Wall: public Actor::Vanilla{
      public:
        Wall() noexcept;
        ~Wall();

        virtual void initAbility() noexcept override=0;

      private:
        void updateActor(const Game::Time&) noexcept override final{}

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };

    class Ceil final: public Wall{
      public:
        Ceil() noexcept=default;
        ~Ceil()=default;

        void initAbility() noexcept override final;
    };

    class Floor final: public Wall{
      public:
        Floor() noexcept=default;
        ~Floor()=default;

        void initAbility() noexcept override final;
    };

    class RightWall final: public Wall{
      public:
        RightWall() noexcept=default;
        ~RightWall()=default;

        void initAbility() noexcept override final;
    };
}
