#pragma once

#include <memory>
#include "Concept.hpp"

namespace My{
    namespace Math{
        namespace Number{
            using Int=int;
            using Real=double;
        }
        namespace Angle{
            struct Radian;
            struct Degree;
        }
        template<Numeric Number> struct Vector2;
    }
}

class MainEngine;

namespace Game{
    using Time=double;
    using Vector2D=My::Math::Vector2<double>;

    enum class Status;
    namespace Core{
        class Engine;
    }
    using Heart=std::shared_ptr<Core::Engine>;

    namespace SubEngine{
        class Interface;
        enum class Type;
        class ActorManager;
        class GameLogic;
        class GraphicsEngine;
        class InputSystem;
        class Logger;
        class PhysicsSimulator;
        class ResourceManager;
        class SoundEngine;
        class Timer;
    }
    using Plugin=std::shared_ptr<SubEngine::Interface>;

    namespace Actor{
        class Interface;
        enum class Type;
        class Player;
        class Enemy;
    }
    using Object=std::shared_ptr<Actor::Interface>;

    namespace Component{
        class Interface;
        enum class Type;
        class AI;
        class ColoredBody;
        class Controllable;
        class Drawable;
        class Movable;
        class Rigidbody;
    }
    using Part=std::shared_ptr<Component::Interface>;

    namespace Blueprint{
        struct Actor;
        struct Player;
        struct Enemy;
    }
}

// Defined in Skin.hpp
namespace Skin{
    struct TrueColor;
    namespace Flyweight{
        struct Rect;
        struct SpinRect;
        struct ColorRect;
    }
    struct Key;
    struct Attribute_2D;
} // namespace Skin

namespace Null{
    namespace SubEngine{
        class ActorManager;
        class GameLogic;
        class GraphicsEngine;
        class InputSystem;
        class PhysicsSimulator;
        class ResourceManager;
        class SoundEngine;
    }
    namespace Actor{
        class Dummy;
    }
    namespace Component{
        class Component;
        class AI;
    }
}

namespace Default{
    namespace SubEngine{
        class PhysicsSimulator_default;
    }
}

namespace WithSTD{
    namespace SubEngine{
        class InputSystem_default;
        class Logger_default;
        class Timer_default;
    }
}

namespace WithSDL{
    namespace Skin{
        namespace Flyweight{
            struct SpriteForSDL;
        }
    }
    namespace Core{
        class Engine;
    }
    namespace SubEngine{
        class GraphicsEngine;
        class InputSystem;
    }
    namespace Component{
        class Sprite;
        class AnimSprite;
        class BGSprite;
    }
}
