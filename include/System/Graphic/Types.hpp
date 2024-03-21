#pragma once

#include <string>
#include "Math.hpp"
#include "Component/Movable/Types.hpp"
#include "Component/Drawable/Types.hpp"

namespace Graphic{
    using namespace Skin;
    using namespace Transform;

    struct Screen{
        My::Math::Vector2<unsigned int> position={}, size={};
    };

    struct Window{
        std::string title="";
        Screen screen={};
    };

    struct ColorBox{
        const TrueColor color;
        const Position2D position;
    };
}

namespace WithSDL{
    using namespace Graphic;

    class SDL_Texture;
}

// painter's algorithm.
namespace Painter{
    using namespace Graphic;
    using namespace WithSDL;

    struct BasicSprite{
        const int drawOrder;
        const ColorBox box;
    };
    struct SpriteWithSDL{
        const SDL_Texture* texture=nullptr;
        const Position2D position;
    };
}