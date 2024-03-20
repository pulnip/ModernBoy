#pragma once

#include <cstdint>

#include <SDL2/SDL_render.h>

#include "Math.hpp"
#include "myfwd.hpp"

namespace Skin{
    using namespace Game;
    using namespace My::Math::Number;
    using namespace My::Math::Angle;

    struct TrueColor {
        using Channel = uint8_t;
        Channel red=0, green=0, blue=0, alpha = 255;
    };
    namespace Flyweight{
        struct Rect{
            const Vector2D& position;
            const Vector2D size;
        };

        struct ColorRect{
            const Rect rect;
            const TrueColor& color;
        };

        struct SpinRect{
            const Rect rect;
            const Radian& rotation;
        };
    }

        Flyweight::Rect rect() const noexcept{
            return{position.linear, volume.size()};
        }
        Flyweight::SpinRect spinRect() const noexcept{
            return{rect(), position.rotation};
        }

        void update(const Time& time){
            position += velocity * time;
        }
        void undo_update(const Time& time){
            position -= velocity * time;
        }

        void set(const Flyweight::Rect& rect){
            position.linear=rect.position;
            volume.base=rect.size;
        }
    };

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
