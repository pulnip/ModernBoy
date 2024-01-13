#pragma once

#include <vector>

#include "Component.hpp"

class CollisionComponent : public Component {
  public:
    void update(const float &deltaTime) noexcept override;

    void allow(const std::weak_ptr<Actor> opponent) noexcept {
        opponents.emplace_back(opponent);
    }

    ComponentName getName() const noexcept override {
        return ComponentName::CollisionComponent;
    }

  protected:
    CollisionComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 200;
    }

  private:
    std::vector<std::weak_ptr<class Actor>> opponents;
};