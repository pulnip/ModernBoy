#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "Skin.hpp"
#include "Math.hpp"

namespace Blueprint{

    struct Actor{
        Skin::Attribute_2D physicsAttribute;
        Skin::TrueColor color;
    };

    struct Setting{
        Window window;
        std::vector<Actor> actors;
    };

    struct LoggerInfo{
        std::string type="", name="";
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