#pragma once

#include "Actor/Actor.hpp"

class Paddle : public Actor,
    public Observable<GameStatus>
{
  public:
    ~Paddle()=default;

  protected:
    Paddle() noexcept=default;

  private:
    void updateActor(const float &deltaTime) noexcept override final;
    void injectDependency() noexcept override final;
};
