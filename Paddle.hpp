#pragma once

#include "Actor.hpp"

class Paddle: public Actor{
public:
    Paddle(class Game* game);
    ~Paddle()=default;

    void UpdateActor(float deltaTime) override;
private:
    class TransformComponent* tc;
    const Vector2 start_position, fixed_velocity, fixed_size;
};
