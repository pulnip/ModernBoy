#pragma once

#include <memory>

#include "gefwd.hpp"
#include "GameEngine/GameEngine.hpp"

class GameEngineWithSDL: public GameEngine,
    public std::enable_shared_from_this<GameEngineWithSDL>
{
  public:
    virtual ~GameEngineWithSDL();

  protected:
    GameEngineWithSDL(std::shared_ptr<Game::Plugin::Logger>& logger) noexcept;

  private:
    void injectDependency() noexcept override final;

  private:
    virtual void setAttribute() noexcept=0;
};