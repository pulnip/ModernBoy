#pragma once

#include "GameEngineWithSDL.hpp"

class TestGame: public GameEngineWithSDL{
  public:
    TestGame() noexcept=default;
    ~TestGame()=default;

  private:
    void setAttribute() noexcept override final;
};