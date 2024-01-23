#pragma once

#include "Property.hpp"
#include "Component.hpp"

struct Attribute_2D;

class MoveComponent final: public Component{
    friend class Actor;
  public:
    MoveComponent() noexcept=default;
    ~MoveComponent()=default;

  private:
    ComponentName getName() const noexcept override final{
        return ComponentName::MoveComponent;
    }
    void update(const float &deltaTime) noexcept override final;
    void injectDependency() noexcept override final{}

    int initUpdateOrder() const noexcept override final{ return 201; }

  public:
    Property<Attribute_2D> attr;
};
