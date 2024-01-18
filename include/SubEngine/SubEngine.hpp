#pragma once

#include "Observable.hpp"
#include "Makable.hpp"
#include "ISubEngine.hpp"

enum class SubEngineName {
    SubEngine,
    ActorManager,
    GameLogic,
    GraphicsEngine,
    InputSystem,
    PhysicsSimulator,
    ResourceManager,
    SoundEngine
};

class SubEngine: public ISubEngine,
    public Makable<SubEngine, IGameEngine>,
    public Observable<Lifetime, ISubEngine>
{
  public:
    virtual ~SubEngine() = default;

  protected:
    SubEngine() noexcept=default;

  private:
    void postConstruct() noexcept override final;

  private:
    virtual SubEngineName getName() const noexcept{
        return SubEngineName::SubEngine;
    }
    virtual void injectDependency() noexcept=0;
    virtual void update(const float &deltaTime) noexcept override=0;
};
