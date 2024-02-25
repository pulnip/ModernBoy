#pragma once

#include "myfwd.hpp"
#include "Actor/Vanilla.hpp"

namespace Test{
    class Ship final: public Actor::Vanilla{
      public:
        Ship() noexcept;
        ~Ship();

        void initAbility() noexcept override final;

      private:
        void updateActor(const Game::Time&) noexcept override final;

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}
