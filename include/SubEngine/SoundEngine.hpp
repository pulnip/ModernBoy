#pragma once

#include "SubEngine.hpp"

class SoundEngine : public SubEngine {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    SoundEngine(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    void postConstruct() noexcept override;
};
