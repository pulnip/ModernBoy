#pragma once

#include "Actor/Actor.hpp"

class CelestialSky final: public Game::Actor::Interface{
  public:
    CelestialSky() noexcept=default;
    ~CelestialSky()=default;

  private:
    void updateActor(const Game::Time& deltaTime) noexcept override final{}
    void postConstruct() noexcept override final;
};