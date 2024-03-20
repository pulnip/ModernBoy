#pragma once

#include <cstdint>

#include <SDL2/SDL_render.h>

#include "Math.hpp"
#include "myfwd.hpp"

namespace Skin{
    using namespace Game;
    using namespace My::Math::Number;
    using namespace My::Math::Angle;

    struct Key{
        enum class Status{
            PRESSED,
            RELEASED
        } status;
        uint8_t key;
    };
}

namespace Game{
    enum class Status{
        FORCE_QUIT,
        GAME_OVER,
        UNEXPECTED
    };
}

namespace WithSDL{
    namespace Skin{
        namespace Flyweight{
            struct Sprite{
                ::Skin::Flyweight::SpinRect spinRect;
                SDL_Texture* texture=nullptr;
            };
        }
    }
}
