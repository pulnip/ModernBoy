#pragma once

#include "SubEngine.hpp"

class SoundEngine: public SubEngine{
  public:
    virtual ~SoundEngine()=default;

  protected:
    SoundEngine() noexcept=default;

  private:
    SubEngineName getName() const noexcept override{
        return SubEngineName::SoundEngine;
    }
    virtual void injectDependency() noexcept override{}

  private:
    virtual void update(const float &deltaTime) noexcept override=0;
};

class NullSoundEngine: public SoundEngine{
  private:
    void update(const float &deltaTime) noexcept override final{}
};
