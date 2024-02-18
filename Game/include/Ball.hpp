#pragma once

#include "Observable.hpp"
#include "Actor/Actor.hpp"

class Ball final: public Game::Actor::Interface,
    public UniqueObservable<Game::Status>
{
  public:
    Ball() noexcept=default;
    ~Ball()=default;

  private:
    void updateActor(const Game::Time& deltaTime) noexcept override;
    void postConstruct() noexcept override;
};
