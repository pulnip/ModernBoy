#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "Makable.hpp"
#include "Math.hpp"
#include "Observable.hpp"
#include "Observer.hpp"
#include "PubSubMessage.hpp"

class Game;
class Actor;

enum class SubEngineName {
    SubEngine,

    ResourceManager,
    InputSystem,
    GameLogic,
    PhysicsSimulator,
    SoundEngine,
    GraphicsEngine,
    ActorManager,
};

// interface

class SubEngine : public std::enable_shared_from_this<SubEngine>, public Makable<SubEngine, Game>, public Observable<PSMSG::Lifetime, std::shared_ptr<SubEngine>> {
  public:
    virtual ~SubEngine() = default;
};

// Resource Manager

class ResourceManager : public SubEngine {
};

class SDL_Texture;

class SDL_ResourceManager : public ResourceManager {
  public:
    std::optional<SDL_Texture *> getTexture(const std::string &fileName) noexcept;

  private:
    SDL_Texture *loadTexture(const std::string &fileName) noexcept;

    std::map<const std::string, SDL_Texture *> textures;
};

// Input System

enum class KeyStatus {
    PRESSED,
    RELEASED
};

class InputSystem : public SubEngine, public Observable<KeyStatus, uint8_t> {
};

// Game Logic

enum class GameStatus {
    GAME_OVER,
    UNEXPECTED
};

class GameLogic : public SubEngine, public Observer<GameStatus> {
  private:
    void onNotify(GameStatus status) override;
};

// Physics Simulator

class PhysicsSimulator : public SubEngine {};

// Sound Engine

class SoundEngine : public SubEngine {};

// Graphics Engine

class TrueColor;
class SDL_Sprite;

class GraphicsEngine : public SubEngine, public Observer<ColorRect> {
  private:
    virtual void onNotify(ColorRect rect) noexcept override = 0;
};

class SDL_GraphicsEngine : public GraphicsEngine, public Observer<SDL_Sprite> {
  private:
    void onNotify(ColorRect rect) noexcept;
    void onNotify(SDL_Sprite sprite) noexcept;
};

struct TrueColor {
    using Channel = uint8_t;
    Channel red = 0, green = 0, blue = 0, alpha = 255;
};
struct ColorRect {
    TrueColor color;
    Vector2 position;
    Math::Radian rotation = 0.0;
};

class SDL_Texture;
struct SDL_Sprite {
    SDL_Texture *texture;
    Vector2 position;
    Math::Radian rotation = 0.0;
};

// Actor Manager

class ActorManager : public SubEngine, public Observer<PSMSG::Lifetime, std::shared_ptr<Actor>> {};
