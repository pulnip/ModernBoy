#pragma once

#include <memory>
#include "myfwd.hpp"
#include "WithSDL/Core.hpp"

class TestGame final: public ::Engine::Core{
  public:
    TestGame(std::shared_ptr<Game::SubEngine::Logger>& logger) noexcept;
    ~TestGame();

  private:
    void postConstruct() noexcept override final;
};