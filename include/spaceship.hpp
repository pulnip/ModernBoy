#pragma once

#include "GameEngineWithSDL.hpp"

class spaceShip : public GameEngineWithSDL {
  public:
    spaceShip() noexcept = default;

  private:
    void postConstruct() noexcept override;
};