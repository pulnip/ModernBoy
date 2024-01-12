#pragma once

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "Makable.hpp"
#include "Math.hpp"
#include "Observable.hpp"
#include "Observer.hpp"
#include "PubSubMessage.hpp"
#include "Skin.hpp"

class GameEngine;

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

// Resource Manager

class ResourceManager : public SubEngine {
  protected:
    ResourceManager(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    void update(const float &deltaTime) noexcept override {}
};

class SDL_Texture;
class SDL_Renderer;

class SDL_ResourceManager : public ResourceManager {
    friend class Game;

  public:
    ~SDL_ResourceManager();

    std::optional<SDL_Texture *> getTexture(const std::string &fileName) noexcept;

  protected:
    SDL_ResourceManager(const OwnerRef owner) noexcept : ResourceManager(owner) {}

  private:
    void postConstruct() noexcept override;

    // 이미지 로딩 과정 캡슐화
    SDL_Texture *loadTexture(const std::string &fileName) noexcept;
    void setContext(const std::shared_ptr<SDL_Renderer *> context) noexcept {
        this->context = context;
    }

  private:
    std::weak_ptr<SDL_Renderer *> context;
    std::map<const std::string, SDL_Texture *> textures;
};

// Input System

enum class GameStatus;

struct Key {
    enum class Status {
        PRESSED,
        RELEASED
    } status;
    uint8_t key;
};

class InputSystem : public SubEngine, public Observable<GameStatus> {
  public:
    virtual void update(const float &deltaTime) noexcept override = 0;

  protected:
    InputSystem(const OwnerRef owner) noexcept : SubEngine(owner) {}
    virtual void postConstruct() noexcept override = 0;

  protected:
    std::map<uint8_t, Observable<Key>> keyMap;
};

class SDL_InputSystem : public InputSystem {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    SDL_InputSystem(const OwnerRef owner) noexcept : InputSystem(owner) {}

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
  public:
    void update(const float &deltaTime) noexcept override;
    bool isReady() const noexcept { return ready; }

  protected:
    GameLogic(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    void postConstruct() noexcept override;
    void onNotify(GameStatus status) noexcept override;

  private:
    bool ready = true;
};

// Physics Simulator

class PhysicsSimulator : public SubEngine, public Observer<Matter> {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    PhysicsSimulator(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    void postConstruct() noexcept override;
    void onNotify(Matter matter) noexcept override;
};

// Sound Engine

class SoundEngine : public SubEngine {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    SoundEngine(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    void postConstruct() noexcept override;
};

// Graphics Engine

class GraphicsEngine : public SubEngine, public Observer<ColorRect> {
  private:
    using Drawable = std::shared_ptr<DrawComponent>;
    struct DrawOrder {
        bool operator()(const Drawable &lhs, const Drawable &rhs) const;
    };

  public:
    virtual ~GraphicsEngine() = default;
    void update(const float &deltaTime) noexcept override;

  protected:
    GraphicsEngine(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    virtual void postConstruct() noexcept override = 0;
    virtual void onNotify(ColorRect rect) noexcept override = 0;

    virtual void prepareRendering() noexcept = 0;
    virtual void finalizeRendering() noexcept = 0;

  private:
    // ordered by Draw Order(Draw Component's);
    std::multiset<Drawable, DrawOrder> drawables;
};

class SDL_GraphicsEngine : public GraphicsEngine, public Observer<SDL_Sprite> {
  public:
    ~SDL_GraphicsEngine();

    void prepareRendering() noexcept override {
        initBackground();
    }
    void finalizeRendering() noexcept override {
        changeColorBuffer();
    }

  protected:
    SDL_GraphicsEngine(const OwnerRef owner) noexcept : GraphicsEngine(owner) {}

  private:
    void postConstruct() noexcept override;

    void onNotify(ColorRect rect) noexcept;
    void onNotify(SDL_Sprite sprite) noexcept;

    void initBackground() noexcept;
    void changeColorBuffer() noexcept;

  private:
    // SDL이 생성한 윈도우
    std::shared_ptr<SDL_Window *> window;
    std::shared_ptr<SDL_Renderer *> renderer;
};

// Actor Manager

class Actor;
class DrawComponent;

class ActorManager : public SubEngine, public Observer<PSMSG::Lifetime, Actor> {
  public:
    std::optional<std::weak_ptr<SubEngine>>
    requestSubEngine(const SubEngineName name) noexcept;
    void update(const float &deltaTime) noexcept override;

  protected:
    ActorManager(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    void postConstruct() noexcept override;
    void onNotify(MSG_t lifetime, spObservable actor) noexcept override;

    void appendActor(const std::shared_ptr<Actor> actor) noexcept;
    void removeActor(const std::shared_ptr<Actor> actor) noexcept;

  private:
    bool isUpdatingActors = false;
    std::vector<std::shared_ptr<Actor>> actors;
    std::vector<std::shared_ptr<Actor>> pendingActors;
};
