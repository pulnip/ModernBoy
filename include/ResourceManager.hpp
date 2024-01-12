#pragma once

#include "SubEngine.hpp"

class ResourceManager : public SubEngine {
  protected:
    ResourceManager(const OwnerRef owner) noexcept : SubEngine(owner) {}

  private:
    void update(const float &deltaTime) noexcept override {}
};
