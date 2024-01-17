#include "p1pingpong.hpp"

void p1pingpong::injectDependency() noexcept {
    GameEngineWithSDL::injectDependency();
#ifdef MUSI
    auto ceil = Actor::make<Wall>(self);
    ceil->position = {1024.0f / 2, 15.0f / 2};
    ceil->baseSize = {1024.0f, 15.0f};

    auto floor = Actor::make<Wall>(self);
    floor->position = {1024.0f / 2, 768.0f - 15.0f / 2};
    floor->baseSize = {1024.0f, 15.0f};

    auto rightWall = Actor::make<Wall>(self);
    rightWall->position = {1024.0f - 15.0f / 2, 768.0f / 2};
    rightWall->baseSize = {15.0f, 768.0f};

    auto paddle = Actor::make<Paddle>(self);
    paddle->allowCollision(ceil);
    paddle->allowCollision(floor);

    auto ball = Actor::make<Ball>(self);
    ball->allowCollision(ceil);
    ball->allowCollision(floor);
    ball->allowCollision(rightWall);
    ball->allowCollision(paddle);
#endif
}
