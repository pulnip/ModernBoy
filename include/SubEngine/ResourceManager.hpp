#pragma once

#include "SubEngine.hpp"

class ResourceManager : public SubEngine {
  protected:
    ResourceManager() noexcept=default;

  private:
    void update(const float &deltaTime) noexcept override {}
        SubEngineName getName() const noexcept override{
        return SubEngineName::ResourceManager;
    }
};
