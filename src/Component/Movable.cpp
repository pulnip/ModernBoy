#include "Component/Movable/Movable.hpp"

Movable::Movable(
    const Transform2D& tf
) noexcept: transform(tf), Component(500){}

void Movable::update(
    const Game::Time& dt, Actor& actor
) noexcept{
    auto& pos = transform.position;
    auto& vel = transform.velocity;

    pos += vel * dt;
}