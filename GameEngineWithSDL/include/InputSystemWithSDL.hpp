#pragma once

#include "SubEngine/InputSystem.hpp"

class InputSystemWithSDL : public InputSystem {
  public:
    ~InputSystemWithSDL()=default;

  protected:
    InputSystemWithSDL() noexcept=default;

  private:
    void update(const float &deltaTime) noexcept override;
    void injectDependency() noexcept override final;
};
