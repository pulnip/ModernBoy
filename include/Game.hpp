#pragma once

#include <list>
#include <map>
#include <memory>
#include <vector>

#include "Makable.hpp"
#include "Observer.hpp"

class SubEngine;

class ResourceManager;
class SDL_ResourceManager;
class InputSystem;
class SDL_InputSystem;
class GameLogic;
class PhysicsSimulator;
class SoundEngine;
class GraphicsEngine;
class SDL_GraphicsEngine;
class ActorManager;

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

class SDL_GameEngine : public GameEngine {
  public:
    ~SDL_GameEngine();

  protected:
    SDL_GameEngine() noexcept;
    virtual void postConstruct() noexcept override;
};

class p1pingpong : public SDL_GameEngine {
  public:
    p1pingpong() noexcept = default;

  private:
    void postConstruct() noexcept override;
};

class spaceShip : public SDL_GameEngine {
  public:
    spaceShip() noexcept = default;

  private:
    void postConstruct() noexcept override;
};