#pragma once

#include "Actor/Actor.hpp"

class Ship final: public Actor{
  public:
    Ship() noexcept=default;
    ~Ship()=default;

  private:
    void updateActor(const float& deltaTime) noexcept override final;
    void injectDependency() noexcept override final;
};
