#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "Component/Movable/Types.hpp"
#include "Component/Drawable/Types.hpp"
#include "Math.hpp"

namespace Blueprint{
    struct Actor{
        Transform::Transform2D physicsAttribute;
        Skin::TrueColor color;
    };

    struct Player: public Actor{
        enum class MoveType{
            // independent to rotation
            UPWARD, DOWNWARD,
            LEFTWARD, RIGHTWARD,
            // dependent to rotation
            FORWARD, BACKWARD,
            JUMP
        };
        std::map<uint8_t, MoveType> keyMap;
    };

    using Predicate=std::function<bool(void)>;
    using Work=std::function<void(void)>;

    struct Enemy: public Actor{
        enum class When{
            OFFSCREEN
        };
        enum class WhatToDo{
            GAME_OVER
        };
        std::map<When, WhatToDo> behaviourMap;
    };
}