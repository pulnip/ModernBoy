#pragma once

#include "Actor/Actor.hpp"

class AnimSpriteComponent;
class InputComponent;
class MoveComponent;

class Ship : public Actor {
  public:
    void updateActor(const float &deltaTime) noexcept override;

  protected:
    Ship() noexcept;

  private:
    void injectDependency() noexcept override;

  private:
    std::shared_ptr<AnimSpriteComponent> sc;
    std::shared_ptr<InputComponent> ic;
    std::shared_ptr<MoveComponent> mc;
};
