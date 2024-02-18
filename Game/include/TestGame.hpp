#pragma once

#include <memory>

#include "gefwd.hpp"
#include "GameEngineWithSDL.hpp"

class TestGame final: public WithSDL::Core::Engine{
  public:
    TestGame(std::shared_ptr<Game::SubEngine::Logger>& logger) noexcept;
    ~TestGame();

  private:
    void postConstruct() noexcept override final;
};