#pragma once

#include <cstdint>
#include "Component/Movable/Types.hpp"

namespace Skin{
    struct TrueColor{
        using Channel = uint8_t;
        Channel red=0, green=0, blue=0, alpha = 255;
    };

    using Vector2D = Transform::Vector2D;
    using Radian = Transform::Radian;

    struct Rect{
        const Vector2D position;
        const Vector2D size;
        const Radian rotation=0.0;
    };

    struct ColorRect{
        const Rect rect;
        const TrueColor color;
    };

    Rect toRect(const Transform::Transform2D& t){
        return {
            t.position.linear,
            t.volume.size(),
            t.position.rotation
        };
    }
}

namespace Traits{
    enum class AnimationType{
        LINEAR,
        ACCEL,
        DECEL
    };
}