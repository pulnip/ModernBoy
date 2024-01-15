#pragma once

#include "Actor/Actor.hpp"

enum class GameStatus;
class BoxComponent;
class CollisionComponent;
class InputComponent;
class MoveComponent;

class Paddle : public Actor, public Observable<GameStatus> {
  public:
    void updateActor(const float &deltaTime) noexcept override;

    void allowCollision(const std::weak_ptr<Actor> opponent) noexcept;

  protected:
    Paddle(const std::weak_ptr<ActorManager> owner) noexcept;

  private:
    void postConstruct() noexcept override;

  private:
    std::shared_ptr<BoxComponent> bc;
    std::shared_ptr<CollisionComponent> cc;
    std::shared_ptr<InputComponent> ic;
    std::shared_ptr<MoveComponent> mc;
};
