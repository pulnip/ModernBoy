#pragma once

#include "SubEngine.hpp"

class SoundEngine : public SubEngine {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    SoundEngine() noexcept=default;

  private:
    void postConstruct() noexcept override;
        SubEngineName getName() const noexcept override{
        return SubEngineName::SoundEngine;
    }

};
