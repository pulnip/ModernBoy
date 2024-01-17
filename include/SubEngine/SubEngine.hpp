#pragma once

#include <memory>

#include "Makable.hpp"
#include "gefwd.hpp"
#include "GameEngine/GameEngine.hpp"

class SubEngine : public Makable<SubEngine, GameEngine> {
  protected:
    using OwnerRef = std::weak_ptr<GameEngine>;

  public:
    virtual ~SubEngine() = default;

    virtual void update(const float &deltaTime) noexcept = 0;
    virtual SubEngineName getName() const noexcept=0;

  protected:
    SubEngine() noexcept=default;
    virtual void postConstruct() noexcept override = 0;
};

enum class SubEngineName {
    ActorManager,
    GameLogic,
    GraphicsEngine,
    InputSystem,
    PhysicsSimulator,
    ResourceManager,
    SoundEngine
};
