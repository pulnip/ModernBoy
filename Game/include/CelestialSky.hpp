#pragma once

#include "Actor/Actor.hpp"

class CelestialSky final: public Actor{
  public:
    CelestialSky() noexcept=default;
    ~CelestialSky()=default;

  private:
    void updateActor(const float& deltaTime) noexcept override final{}
    void injectDependency() noexcept override final;
};