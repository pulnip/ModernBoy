#pragma once

#include "Component.hpp"
#include "Math.hpp"

class MoveComponent : public Component {
  public:
    void update(const float &deltaTime) noexcept override;

    ComponentName getName() const noexcept override {
        return ComponentName::MoveComponent;
    }

  protected:
    MoveComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 201;
    }

  public:
    // unit per second
    Vector2 velocity;
    Math::Radian rotationVelocity;
};
