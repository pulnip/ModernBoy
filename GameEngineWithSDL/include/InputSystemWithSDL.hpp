#pragma once

#include "SubEngine/InputSystem.hpp"

class InputSystemWithSDL final: public InputSystem {
  public:
    InputSystemWithSDL() noexcept=default;
    ~InputSystemWithSDL()=default;

  private:
    void update(const float &deltaTime) noexcept override;
    void setAttribute() noexcept override final{}

    uint8_t escapeKeycode() const noexcept override final;
};
