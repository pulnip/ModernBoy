#pragma once

#include "Property.hpp"
#include "Math.hpp"
#include "Skin.hpp"
#include "Component.hpp"

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
    Attribute_2D attr;
};
