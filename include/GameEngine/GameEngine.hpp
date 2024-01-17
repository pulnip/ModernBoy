#pragma once

#include <map>
#include <functional>

#include "Makable.hpp"
#include "IGameEngine.hpp"

class GameEngine: public IGameEngine,
    public Makable<GameEngine>
{
  public:
    virtual ~GameEngine() = default;


  protected:
    GameEngine() = default;

  private:
    void postConstruct() noexcept override final;
    void onNotify(MSG_t msg, spObservable se) noexcept override final;
    void run() noexcept override final;
    std::shared_ptr<SubEngine>
    query(const SubEngineName name) noexcept override final;

  private:
    virtual void injectDependency() noexcept=0;

  protected:
    std::shared_ptr<ResourceManager> resourceManager;
    std::shared_ptr<InputSystem> inputSystem;
    std::shared_ptr<GameLogic> gameLogic;
    std::shared_ptr<ActorManager> actorManager;
    std::shared_ptr<GraphicsEngine> graphicsEngine;
    std::map<SubEngineName, std::shared_ptr<SubEngine>> subEngines;

    std::shared_ptr<Timer> timer;
};
