#pragma once

#include <map>
#include <functional>

#include "Makable.hpp"
#include "IGameEngine.hpp"
#include "Observer.hpp"

class GameEngine: public IGameEngine,
    public Makable<GameEngine>,
    public Observer<GameStatus>
{
  public:
    using SubEngineMap=std::map<SubEngineName, std::shared_ptr<ISubEngine>>;

  public:
    virtual ~GameEngine() = default;

  protected:
    GameEngine() = default;

  private:
    void postConstruct() noexcept override final;
    void onNotify(Lifetime msg, std::shared_ptr<ISubEngine> se) noexcept override final;
    void onNotify(GameStatus status) noexcept override final;

    void run() noexcept override final;
    std::shared_ptr<ISubEngine>
    find(const SubEngineName name) noexcept override final;

  private:
    virtual void injectDependency() noexcept=0;

  protected:
    SubEngineMap subEngines;
    std::shared_ptr<Timer> timer;

  private:
    bool isRunning=false;
};
