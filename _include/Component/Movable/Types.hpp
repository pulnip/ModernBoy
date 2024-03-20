#pragma once

#include "Math.hpp"
#include "Engine/Types.hpp"

namespace Transform{
    using namespace My::Math;

    using Vector2D = Vector2<Number::Real>;
    using Radian = Angle::Radian;

    struct Volume{
        Vector2D base;
        Number::Real scale=1.0;

        Vector2D size() const noexcept{
            return base*scale;
        }
    };

    struct PhysicsElement{
        Vector2D linear;
        Radian rotation=0.0;

        PhysicsElement& operator+=(const PhysicsElement& other) noexcept{
            linear+=other.linear;
            rotation.get()+=other.rotation.get();
            return *this;
        }
        PhysicsElement& operator-=(const PhysicsElement& other) noexcept{
            linear-=other.linear;
            rotation.get()-=other.rotation.get();
            return *this;
        }
    };

    struct Position: public PhysicsElement{};
    struct Velocity: public PhysicsElement{};

    explicit Position operator*(const Velocity& v, const Game::Time& t){
        return { v.linear*t, v.rotation.get()*t };
    }

    struct Transform2D{
        Transform2D() noexcept=default;
        Transform2D(const Vector2D& position, const Vector2D& size) noexcept:
        position{position, 0.0}, volume{size, 0.0}{}

        Volume volume;
        Position position;
        Velocity velocity;
    };
}