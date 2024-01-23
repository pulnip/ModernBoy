#pragma once

#include "Actor/Actor.hpp"

class Asteroid final: public Actor{
  public:
    Asteroid() noexcept=default;
    ~Asteroid()=default;

  private:
    void updateActor(const float&) noexcept override final{}
    void injectDependency() noexcept override final;
};
