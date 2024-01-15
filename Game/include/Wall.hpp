#pragma once

#include "Actor/Actor.hpp"

class BoxComponent;
class MoveComponent;

class Wall : public Actor {
  private:
    Wall(const std::weak_ptr<ActorManager> owner) noexcept;

  protected:
    void postConstruct() noexcept override;

  private:
    std::shared_ptr<BoxComponent> bc;
    std::shared_ptr<MoveComponent> mc;
};
