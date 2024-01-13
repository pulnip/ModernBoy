#pragma once

#include <memory>

#include "Makable.hpp"

class ResourceManager;
class InputSystem;
class GameLogic;
class ActorManager;
class GraphicsEngine;

class GameEngine : public Makable<GameEngine>, public std::enable_shared_from_this<GameEngine> {
  public:
    GameEngine() = default;
    virtual ~GameEngine() = default;

    void run() noexcept;

  private:
    virtual void postConstruct() noexcept override = 0;

  protected:
    std::shared_ptr<ResourceManager> resourceManager;
    std::shared_ptr<InputSystem> inputSystem;
    std::shared_ptr<GameLogic> gameLogic;
    std::shared_ptr<ActorManager> actorManager;
    std::shared_ptr<GraphicsEngine> graphicsEngine;
};
