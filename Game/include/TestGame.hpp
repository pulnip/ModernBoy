#pragma once

#include "GameEngineWithSDL.hpp"

class TestGame: public GameEngineWithSDL{
  private:
    void injectDependency() noexcept override final;
};