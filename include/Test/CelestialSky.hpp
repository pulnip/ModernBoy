#pragma once

#include "myfwd.hpp"
#include "Actor/Vanilla.hpp"

namespace Test{
    class CelestialSky final: public Actor::Vanilla{
      public:
        CelestialSky() noexcept;
        ~CelestialSky();

        void initAbility() noexcept override final;

      private:
        void updateActor(const Game::Time&) noexcept override final{}

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}
