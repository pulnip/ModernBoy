#pragma once

#include <vector>

#include "Component.hpp"
#include "gefwd.hpp"

class CollisionComponent : public Component {
  public:
    void update(const float &deltaTime) noexcept override;

    void allow(const std::weak_ptr<IActor> opponent) noexcept {
        opponents.emplace_back(opponent);
    }

    ComponentName getName() const noexcept override {
        return ComponentName::CollisionComponent;
    }

  protected:
    CollisionComponent() noexcept{
        updateOrder = 200;
    }
    void injectDependency() noexcept override{}


  private:
    std::vector<std::weak_ptr<IActor>> opponents;
};