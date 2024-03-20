#pragma once

#include "Math.hpp"
#include "Component/Movable/Types.hpp"
#include "Component/Drawable/Types.hpp"

namespace Graphic{
    using namespace Skin;
    using namespace Transform;

    struct ColorBox{
        const TrueColor color;
        const Position position;
    };

    namespace Painter{
        struct BasicSprite{
            // used in painter's algorithm.
            const int drawOrder;
            const ColorBox box;
        };
    }


    struct Screen{
        My::Math::Vector2<unsigned int> position={}, size={};
    };

    struct Window{
        std::string title="";
        Screen screen={};
    };
}