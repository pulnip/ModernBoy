#pragma once

#include <memory>

#include "gefwd.hpp"
#include "GameEngineWithSDL.hpp"

class TestGame final: public GameEngineWithSDL{
  public:
    TestGame(std::shared_ptr<Game::Plugin::Logger>& logger) noexcept;
    ~TestGame();

  private:
    void setAttribute() noexcept override final;
};