#pragma once

#include "Actor/Actor.hpp"

class SpriteComponent;
class MoveComponent;

class Asteroid : public Actor {
  public:
    Asteroid(const std::weak_ptr<ActorManager> owner) noexcept;

  private:
    void postConstruct() noexcept override;

  private:
    std::shared_ptr<SpriteComponent> sc;
    std::shared_ptr<MoveComponent> mc;
};
