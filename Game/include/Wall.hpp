#pragma once

#include "Actor/Actor.hpp"

class BoxComponent;
class MoveComponent;

class Wall : public Actor {
  private:
    Wall() noexcept=default;

  protected:
    void injectDependency() noexcept override;

  private:
    std::shared_ptr<BoxComponent> bc;
    std::shared_ptr<MoveComponent> mc;
};
