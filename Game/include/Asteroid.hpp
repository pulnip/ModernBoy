#pragma once

#include "Actor/Actor.hpp"

class Asteroid final: public Game::Actor::Interface{
  public:
    Asteroid() noexcept=default;
    ~Asteroid()=default;

  private:
    void updateActor(const Game::Time&) noexcept override final{}
    void postConstruct() noexcept override final;
};
