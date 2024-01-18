#pragma once

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
    public Makable<SubEngine, IGameEngine>
{
  public:
    virtual ~SubEngine() = default;
    std::optional<std::shared_ptr<ISubEngine>>
    query(const SubEngineName name) noexcept final;

  protected:
    SubEngine() noexcept=default;

  private:
    void postConstruct() noexcept override final{ injectDependency(); }

  private:
    virtual void update(const float &deltaTime) noexcept override= 0;
    virtual SubEngineName getName() const noexcept{
        return SubEngineName::SubEngine;
    }

  private:
    virtual void injectDependency() noexcept=0;
};
