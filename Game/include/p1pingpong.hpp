#pragma once

#include "GameEngineWithSDL.hpp"

class p1pingpong : public GameEngineWithSDL {
  public:
    p1pingpong() noexcept = default;

  private:
    void injectDependency() noexcept override final;
};