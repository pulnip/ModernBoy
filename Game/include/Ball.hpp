#pragma once

#include "Actor/Actor.hpp"

enum class GameStatus;

class GameLogic;
class CollisionComponent;
class AnimSpriteComponent;
class MoveComponent;

class Ball : public Actor, public Observable<GameStatus> {
  public:
    void updateActor(const float &deltaTime) noexcept override;
    void allowCollision(const std::weak_ptr<Actor> opponent) noexcept;

  protected:
    Ball() noexcept;

  private:
    void injectDependency() noexcept override;

  private:
    std::weak_ptr<GameLogic> gl;

    std::shared_ptr<AnimSpriteComponent> sc;
    std::shared_ptr<CollisionComponent> cc;
    std::shared_ptr<MoveComponent> mc;
};
