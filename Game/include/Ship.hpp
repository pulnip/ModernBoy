#pragma once

#include "Actor/Actor.hpp"

class Ship: public Actor{
  public:
    ~Ship()=default;

  protected:
    Ship() noexcept=default;

  private:
    void updateActor(const float& deltaTime) noexcept override final;
    void injectDependency() noexcept override final;
};
