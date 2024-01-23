#pragma once

#include "GameEngine/GameEngine.hpp"

class GameEngineWithSDL: public GameEngine,
    public std::enable_shared_from_this<GameEngineWithSDL>
{
  public:
    virtual ~GameEngineWithSDL();

  protected:
    GameEngineWithSDL() noexcept;

  private:
    void injectDependency() noexcept override final;

  private:
    virtual void setAttribute() noexcept=0;
};