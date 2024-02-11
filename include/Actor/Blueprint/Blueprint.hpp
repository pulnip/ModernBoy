#pragma once

#include "Skin.hpp"

namespace Blueprint{
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
        } moveType;
    };

    struct Enemy: public Actor{

    };
}