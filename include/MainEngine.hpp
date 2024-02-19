#pragma once

#include <memory>

#include "Makable.hpp"
#include "gefwd.hpp"

class MainEngine: public Makable<MainEngine>{
  public:
    MainEngine() noexcept;

    void setEngine(Game::Heart engine) noexcept{ gameEngine=engine; }
    Game::Heart getEngine() noexcept{ return gameEngine; }
    std::shared_ptr<Game::SubEngine::Logger>
    getLogger() noexcept{ return logger; }

  private:
    Game::Heart gameEngine;
    std::shared_ptr<Game::SubEngine::Logger> logger;
};
