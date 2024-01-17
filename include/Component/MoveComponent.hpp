#pragma once

#include "Component.hpp"
#include "Property.hpp"
#include "Math.hpp"

class MoveComponent : public Component {
    friend class Actor;
  public:
    void update(const float &deltaTime) noexcept override;

    ComponentName getName() const noexcept override {
        return ComponentName::MoveComponent;
    }

  protected:
    MoveComponent() noexcept{
        updateOrder = 201;
    }
    void injectDependency() noexcept override{}

  public:
    Property<Vector2> position;
    Property<Vector2> baseSize;
    Property<Math::Real> scale = 1.0;
    Property<Math::Radian> rotation = 0.0;
    // unit per second
    Property<Vector2> velocity;
    Property<Math::Radian> rotationVelocity;
};
