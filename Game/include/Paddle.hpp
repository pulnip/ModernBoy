#pragma once

#include "Observable.hpp"
#include "Actor/Actor.hpp"

class Paddle final: public Game::Actor::Interface,
    public Observable<Game::Status>
{
  public:
    Paddle() noexcept=default;
    ~Paddle()=default;

  private:
    void updateActor(const Game::Time& deltaTime) noexcept override final;
    void postConstruct() noexcept override final;
};
