#pragma once

#include <map>
#include <functional>

#include "Ownership.hpp"
#include "Makable.hpp"
#include "IGameEngine.hpp"

enum class EngineCommand{
    QUIT
};

class GameEngine: public IGameEngine,
    public Makable<GameEngine>,
    public Owner<SubEngine>,
    public Receiver
{
  public:
    using SubEngineMap=std::map<SubEngineName, std::shared_ptr<ISubEngine>>;

  public:
    virtual ~GameEngine() = default;

  protected:
    GameEngine() = default;

  private:
    void run() noexcept override final;
    std::shared_ptr<ISubEngine>
    find(const SubEngineName name) noexcept override final;

    void postConstruct() noexcept override final{ injectDependency(); }
    void onNotify(Lifetime msg, std::shared_ptr<SubEngine> se) noexcept override final;
    void handler() noexcept override final;

  private:
    virtual void injectDependency() noexcept=0;

  protected:
    SubEngineMap subEngines;
    std::shared_ptr<Timer> timer;

  private:
    bool isRunning=false;
};
