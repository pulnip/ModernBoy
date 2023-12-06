#pragma once

#include "Actor.hpp"

class Paddle: public Actor{
public:
    Paddle(class Game* game);
    ~Paddle()=default;

    void UpdateActor(float deltaTime) override;
public:
    class CollisionComponent* cc;
private:
    class TransformComponent* tc;
    const Vector2 start_position, fixed_velocity, fixed_size;
};

class Wall: public Actor{
public:
    Wall(class Game* game, int x, int y, int w, int h);
    ~Wall()=default;

    void UpdateActor(float deltaTime) override{}
public:
    class CollisionComponent* cc;
};