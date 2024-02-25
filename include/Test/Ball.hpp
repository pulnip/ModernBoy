#pragma once

#include "myfwd.hpp"
#include "Actor/Vanilla.hpp"

namespace Test{
    class Ball final: public Actor::Vanilla{
      public:
        Ball() noexcept;
        ~Ball();

        void initAbility() noexcept override final;

      private:
        void updateActor(const Game::Time&) noexcept override final;

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}
