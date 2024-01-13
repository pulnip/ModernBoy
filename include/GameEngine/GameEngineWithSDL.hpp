#pragma once

#include "GameEngine.hpp"

class GameEngineWithSDL : public GameEngine {
  public:
    ~GameEngineWithSDL();

  protected:
    GameEngineWithSDL() noexcept;
    virtual void postConstruct() noexcept override;
};