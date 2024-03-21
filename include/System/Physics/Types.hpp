#pragma once

#include "Math.hpp"

namespace Physics{
    using namespace My::Math;

    using Unit = Number::Real;
    using Length = Unit;
    using Radian = Angle::Radian;
    using Time = Unit;
    using Mass = Unit;

    using Vector2D = Linear<Unit>;

    struct Element2D{
        Linear<Unit> linear;
        Radian rotation=0.0;

        Element2D& operator+=(const Element2D& other) noexcept{
            linear+=other.linear;
            rotation.get()+=other.rotation.get();
            return *this;
        }
        Element2D& operator-=(const Element2D& other) noexcept{
            linear-=other.linear;
            rotation.get()-=other.rotation.get();
            return *this;
        }
    };

    struct Position2D: public Element2D{};
    struct Velocity2D: public Element2D{};
    
    explicit Position2D operator*(const Velocity2D& v, const Time& t){
        return { v.linear*t, v.rotation.get()*t };
    }

    namespace Collision{
        struct Box2D: public Vector2D{};
        using Circle = Unit;
    }

    struct ObjectA2D{
        ObjectA2D() noexcept=default;
        ObjectA2D(const Vector2D& position, const Vector2D& size) noexcept:
        position{position, 0.0}, detector{size}{}

        Position2D position;
        Velocity2D velocity;
        Collision::Box2D detector;
    };

    struct ObjectC2D{
        ObjectC2D() noexcept=default;
        ObjectC2D(const Vector2D& position, const Unit& size) noexcept:
        position{position, 0.0}, detector{size}{}

        Position2D position;
        Velocity2D velocity;
        Collision::Circle detector;
    };
}