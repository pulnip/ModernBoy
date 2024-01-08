#pragma once

#include <memory>

#include "Makable.hpp"
#include "Observable.hpp"
#include "Observer.hpp"

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

class SubEngine : public std::enable_shared_from_this<SubEngine>, public Makable<SubEngine, Game>, public Observer<Actor>, public Observable<SubEngine> {
};

class ActorManager : public SubEngine {};
