#pragma once

#include <memory>

#include "Makable.hpp"

#include "GameEngine/GameEngine.hpp"

enum class SubEngineName {
    SubEngine,

    ResourceManager,
    InputSystem,
    GameLogic,
    PhysicsSimulator,
    SoundEngine,
    GraphicsEngine,
    ActorManager
};

class SubEngine : public Makable<SubEngine, GameEngine> {
  protected:
    using OwnerRef = std::weak_ptr<GameEngine>;

  public:
    virtual ~SubEngine() = default;

    virtual void update(const float &deltaTime) noexcept = 0;

  protected:
    SubEngine(const OwnerRef owner) noexcept : owner(owner) {}
    virtual void postConstruct() noexcept override = 0;

  private:
    OwnerRef owner;
};
