#pragma once

#include "InputSystem.hpp"

class InputSystemWithSDL : public InputSystem {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    InputSystemWithSDL(const OwnerRef owner) noexcept : InputSystem(owner) {}

  private:
    void postConstruct() noexcept override;
};
