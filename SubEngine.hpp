#pragma once

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>

#include "Makable.hpp"
#include "Math.hpp"
#include "Observable.hpp"
#include "Observer.hpp"
#include "PubSubMessage.hpp"
#include "Skin.hpp"

class GameEngine;
class Actor;

enum class SubEngineName {
    SubEngine,

    ResourceManager,
    SDL_ResourceManager,
    InputSystem,
    GameLogic,
    PhysicsSimulator,
    SoundEngine,
    GraphicsEngine,
    SDL_GraphicsEngine,
    ActorManager
};

// interface

class SubEngine : public std::enable_shared_from_this<SubEngine>, public Makable<SubEngine, GameEngine>, public Observable<PSMSG::Lifetime, std::shared_ptr<SubEngine>> {
  public:
    virtual ~SubEngine() = default;

    virtual void update(const float &deltaTime) noexcept = 0;

  protected:
    SubEngine(const std::weak_ptr<GameEngine> owner) noexcept;
    virtual void postConstruct() noexcept override;

  private:
    std::weak_ptr<GameEngine> owner;
};

// Resource Manager

template <typename Skin>
class ResourceManager : public SubEngine {
  public:
    virtual std::optional<Skin> getSkin(const std::string &fileName) noexcept = 0;

  protected:
    std::map<const std::string, Skin> skins;
};

class SDL_Texture;
class SDL_Renderer;

class SDL_ResourceManager : public ResourceManager<SDL_Texture *> {
    friend class Game;

  public:
    ~SDL_ResourceManager();

    std::optional<SDL_Texture *> getSkin(const std::string &fileName) noexcept override;

  protected:
    SDL_ResourceManager(const std::weak_ptr<GameEngine> owner) noexcept;

  private:
    void postConstruct() noexcept override;

    // 이미지 로딩 과정 캡슐화
    SDL_Texture *loadTexture(const std::string &fileName) noexcept;
    void setContext(const std::shared_ptr<SDL_Renderer *> context) noexcept {
        this->context = context;
    }

  private:
    std::weak_ptr<SDL_Renderer *> context;
};

// Input System

enum class GameStatus;

enum class KeyStatus {
    PRESSED,
    RELEASED
};

class InputSystem : public SubEngine, public Observable<GameStatus> {
  public:
    virtual void update(const float &deltaTime) noexcept override = 0;

  protected:
    InputSystem(const std::weak_ptr<GameEngine> owner) noexcept;

  private:
    virtual void postConstruct() noexcept override = 0;

  protected:
    std::map<uint8_t, Observable<KeyStatus, uint8_t>> keyMap;
};

class SDL_InputSystem : public InputSystem {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    SDL_InputSystem(const std::weak_ptr<GameEngine> owner) noexcept;

  private:
    void postConstruct() noexcept override;
};

// Game Logic

enum class GameStatus {
    GAME_OVER,
    FORCE_QUIT,
    UNEXPECTED
};

class GameLogic : public SubEngine, public Observer<GameStatus> {
  protected:
    GameLogic(const std::weak_ptr<GameEngine> owner) noexcept;

  private:
    void postConstruct() noexcept override;
    void onNotify(GameStatus status) override;
};

// Physics Simulator

class PhysicsSimulator : public SubEngine {};

// Sound Engine

class SoundEngine : public SubEngine {};

// Graphics Engine

class GraphicsEngine : public SubEngine, public Observer<ColorRect> {
  private:
    virtual void postConstruct() noexcept override = 0;
    virtual void onNotify(ColorRect rect) noexcept override = 0;
};

class SDL_GraphicsEngine : public GraphicsEngine, public Observer<SDL_Sprite> {
  public:
    ~SDL_GraphicsEngine();

    void initBackground() noexcept;
    void changeColorBuffer() noexcept;

  protected:
    SDL_GraphicsEngine(const std::weak_ptr<GameEngine> owner) noexcept;

  private:
    void postConstruct() noexcept override;

    void onNotify(ColorRect rect) noexcept;
    void onNotify(SDL_Sprite sprite) noexcept;

  private:
    // SDL이 생성한 윈도우
    std::shared_ptr<SDL_Window *> window;
    std::shared_ptr<SDL_Renderer *> renderer;
};

// Actor Manager

class ActorManager : public SubEngine, public Observer<PSMSG::Lifetime, std::shared_ptr<Actor>> {
  public:
    std::optional<std::weak_ptr<SubEngine>>
    requestSubEngine(const SubEngineName name) noexcept;

#error "draw order, update order 고려해야함!"

  private:
    void postConstruct() noexcept override;
    void onNotify(PSMSG::Lifetime lifetime, std::shared_ptr<Actor> actor) noexcept override;

  private:
    struct updateOrder {
        bool operator()(const std::shared_ptr<Actor> &lhs, const std::shared_ptr<Actor> &rhs) const {
            return lhs->
        }
    };
    std::multiset <
};
