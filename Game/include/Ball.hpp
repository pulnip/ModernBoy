#pragma once

#include "Actor/Actor.hpp"

class Ball final: public Actor,
    public UniqueObservable<GameStatus>
{
  public:
    Ball() noexcept=default;
    ~Ball()=default;

  private:
    void updateActor(const float &deltaTime) noexcept override;
    void injectDependency() noexcept override;
};
