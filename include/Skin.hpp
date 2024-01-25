#pragma once

#include <cstdint>

#include <SDL2/SDL_render.h>

#include "Math.hpp"

struct TrueColor {
    using Channel = uint8_t;
    Channel red=0, green=0, blue=0, alpha = 255;
};

struct Rect{
    Vector2 position;
    Vector2 size;
};

struct ColorRect{
    Rect rect;
    TrueColor color;
};

struct SpinRect{
    Rect rect;
    Math::Radian rotation=0.0;
};

struct SpriteForSDL{
    SpinRect spinRect;
    SDL_Texture *texture=nullptr;
};

struct Attribute_2D{
    struct Volume{
        Vector2 base;
        Math::Real scale=1.0;

        Vector2 size() const noexcept{ return base*scale; }
    } volume;
    struct Position{
        Vector2 linear;
        Math::Radian rotation=0.0;

        Position& operator+=(const Position& other) noexcept{
            linear+=other.linear;
            rotation+=other.rotation;
            return *this;
        }
        Position& operator-=(const Position& other) noexcept{
            linear-=other.linear;
            rotation-=other.rotation;
            return *this;
        }
    } position;
    // unit per second
    struct Velocity{
        Vector2 linear;
        Math::Radian rotation=0.0;
        Position operator*(const Math::Real& time) noexcept{
            return {linear*time, rotation*time};
        }
    } velocity;

    Rect rect() const noexcept{ return{position.linear, volume.size()}; }
    SpinRect spinRect() const noexcept{ return{rect(), position.rotation}; }

    void update(const Math::Real& time){
        position += velocity * time;
    }
    void undo_update(const Math::Real& time){
        position -= velocity * time;
    }

    void set(const Rect& rect){
        position.linear=rect.position;
        volume.base=rect.size;
    }
};
