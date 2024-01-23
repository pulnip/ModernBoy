#pragma once

#include "Actor/Actor.hpp"

class Paddle final: public Actor,
    public Observable<GameStatus>
{
  public:
    Paddle() noexcept=default;
    ~Paddle()=default;

  private:
    void updateActor(const float &deltaTime) noexcept override final;
    void injectDependency() noexcept override final;
};
