#pragma once

#include "Actor/Actor.hpp"

class Ship final: public Game::Actor::Interface{
  public:
    Ship() noexcept=default;
    ~Ship()=default;

  private:
    void updateActor(const Game::Time& deltaTime) noexcept override final;
    void postConstruct() noexcept override final;
};
