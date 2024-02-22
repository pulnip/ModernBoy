#pragma once

#include <memory>

#include "Concept.hpp"

namespace My{
    namespace Math{
        template<Numeric N> struct Vector2;
    }
}

class MainEngine;

namespace Engine{
    struct Screen;
    class Core;
    class ActorManager;
    class GameLogic;
    class Graphics;
    class InputSystem;
    class Logger;
    struct BindedLogger;
    class PhysicsSimulator;
    namespace ResourceManager{}
    namespace SoundEngine{}
    class Timer;
}

namespace Actor{
    class Vanilla;
    enum class Role;
    class Null;
}
using pActor=std::shared_ptr<Actor::Vanilla>;

namespace Component{
    class Ability;
    enum class Type;
    class Null;
    class AI;
    class Controllable;
    class Drawable;
    class Movable;
    class Rigidbody;
}
using pComponent=std::shared_ptr<Component::Ability>;

namespace Game{
    using Time=double;
    using Vector2D=My::Math::Vector2<double>;
    enum class Status;
}

namespace Skin{
    struct TrueColor;
    struct Attribute_2D;
    struct Key;
}

namespace Blueprint{
    struct Screen;
}