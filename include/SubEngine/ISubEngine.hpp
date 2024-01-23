#pragma once

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

class ISubEngine{
  public:
    virtual ~ISubEngine()=default;

  protected:
    ISubEngine() noexcept=default;

  public:
    virtual SubEngineName getName() const noexcept=0;
    virtual void update(const float& deltaTime) noexcept=0;
};
