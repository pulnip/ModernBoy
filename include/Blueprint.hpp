#pragma once

#include <functional>
#include <map>
#include <vector>

#include "Skin.hpp"

namespace Blueprint{
    struct Screen{
        unsigned int x, y;
        unsigned int width, height;
    };

    struct LoggerInfo{
        std::string type, name;
    };

    using namespace Skin;

    struct Actor{
        Attribute_2D physicsAttribute;
        TrueColor color;
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