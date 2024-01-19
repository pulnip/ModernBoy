#pragma once

#include "Component.hpp"
#include "Property.hpp"
#include "Math.hpp"

class MoveComponent: public Component{
    friend class Actor;
  public:
    virtual ~MoveComponent()=default;

  protected:
    MoveComponent() noexcept=default;

    virtual void injectDependency() noexcept override;

  private:
    void update(const float &deltaTime) noexcept override final;
    ComponentName getName() const noexcept override final{
        return ComponentName::MoveComponent;
    }
    int initUpdateOrder() const noexcept override final{ return 201; }

  public:
    Property<Vector2> position;
    Property<Vector2> baseSize;
    Property<Math::Real> scale = 1.0;
    Property<Math::Radian> rotation = 0.0;
    // unit per second
    Property<Vector2> velocity;
    Property<Math::Radian> rotationVelocity;
};
