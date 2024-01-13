#pragma once

#include "Component.hpp"

class AIComponent : public Component {
    enum State {
        Death,
        Patrol,
        Attack
    };

  public:
    void update(const float &deltaTime) noexcept override{
#warning "NOT DEFINED"
    }

    ComponentName getName() const noexcept override {
        return ComponentName::AIComponent;
    }

  protected:
    AIComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 150;
    }
};
