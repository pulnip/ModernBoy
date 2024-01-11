#pragma once

#include <list>
#include <map>
#include <memory>
#include <vector>

#include "Makable.hpp"
#include "Observer.hpp"

class SubEngine;

class SDL_ResourceManager;
class InputSystem;
class SDL_InputSystem;
class GameLogic;
class PhysicsSimulator;
class SoundEngine;
class GraphicsEngine;
class SDL_GraphicsEngine;
class ActorManager;

class GameEngine : public std::enable_shared_from_this<GameEngine>, public Makable<GameEngine>, public Observer<SubEngine> {
};

class SDL_GameEngine : public GameEngine {
  public:
    ~SDL_GameEngine();

    // 게임이 끝나기 전까지 게임 루프를 실행
    void runLoop() noexcept;
    // 게임 종료
    void shutDown() noexcept;

    void quit() noexcept { isRunning = false; }
    class SDL_Texture *getTexture(const char *fileName) noexcept;
    void appendActor(const std::shared_ptr<class Actor> actor) noexcept;
    void appendDrawable(const std::weak_ptr<class DrawComponent> drawable) noexcept;
    void removeDrawable(const std::weak_ptr<class DrawComponent> drawable) noexcept;

  protected:
    SDL_GameEngine() noexcept;
    virtual void postConstruct() noexcept override;

  private:
    // 게임 루프를 위한 헬퍼 함수
    void processInput() noexcept;
    void updateGame() noexcept;
    void generateOutput() noexcept;

    // 이미지 로딩 과정 캡슐화
    class SDL_Texture *loadTexture(const char *fileName) noexcept;

  private:
    std::shared_ptr<SDL_ResourceManager> resourceManager;
    std::shared_ptr<SDL_InputSystem> inputSystem;
    std::shared_ptr<GameLogic> game;
    std::shared_ptr<SDL_GraphicsEngine> graphicsEngine;

    const uint8_t *keyState;

    // 게임이 계속 실행돼야 하는지를 판단
    bool isRunning = false;
    uint32_t ticksCount = 0;

    std::list<std::shared_ptr<class Actor>> actors;
    std::list<std::shared_ptr<class Actor>> pendingActors;
    bool isUpdatingActors = false;
    std::vector<std::weak_ptr<class DrawComponent>> drawables;
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