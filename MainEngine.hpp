#pragma once

#include <memory>

class MainEngine{
  public:
    MainEngine() noexcept;

    std::shared_ptr<class Logger> getLogger() noexcept{ return logger; }

  private:
    std::shared_ptr<class Logger> logger;
    // std::shared_ptr<class GameEngine> gameEngine;
};
