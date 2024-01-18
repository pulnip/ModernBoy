#pragma once

#include "SubEngine/InputSystem.hpp"

class InputSystemWithSDL : public InputSystem {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    InputSystemWithSDL() noexcept=default;

  private:
    void injectDependency() noexcept override;
};
