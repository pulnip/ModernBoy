#pragma once

// Defined in Math.hpp
struct Vector2;

// Defined in PubSubMessage.hpp 
enum class GameStatus;

enum class Lifetime;

// Defined in Skin.hpp
struct TrueColor;
struct Rect;
struct SpinRect;
struct ColorRect;
struct SpriteForSDL;
struct Attribute_2D;

class IGameEngine;
class GameEngine;

class ISubEngine;
class SubEngine;
enum class SubEngineName;
class ActorManager;
class NullActorManager;
class GameLogic;
class NullGameLogic;
class GraphicsEngine;
class NullGraphicsEngine;
class InputSystem;
struct Key;
class NullInputSystem;
class InputSystem_default;
class PhysicsSimulator;
class NullPhysicsSimulator;
class PhysicsSimulator_default;
class ResourceManager;
class NullResourceManager;
class SoundEngine;
class NullSoundEngine;
class Timer;
class Timer_default;

class IActor;
class Actor;

class IComponent;
class Component;
enum class ComponentName;
class CollisionComponent;
class DrawComponent;
class BoxComponent;
class InputComponent;
class MoveComponent;
