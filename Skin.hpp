#pragma once

#include <cstdint>

#include <SDL2/SDL_render.h>

#include "Math.hpp"

struct TrueColor {
    using Channel = uint8_t;
    Channel red = 0, green = 0, blue = 0, alpha = 255;
};

struct ColorRect {
    Vector2 position;
    Vector2 size;
    Math::Radian rotation = 0.0;
    TrueColor color;
};

struct SDL_Sprite {
    Vector2 position;
    Vector2 size;
    Math::Radian rotation = 0.0;
    SDL_Texture *texture;
};
