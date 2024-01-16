#pragma once

#include "Actor/Actor.hpp"

class SpriteComponent;
class MoveComponent;

class Asteroid : public Actor {
  public:
    Asteroid() noexcept;

  private:
    void injectDependency() noexcept override;

  private:
    std::shared_ptr<SpriteComponent> sc;
    std::shared_ptr<MoveComponent> mc;
};
