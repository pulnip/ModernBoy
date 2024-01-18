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

  private:
    virtual void injectDependency() noexcept override=0;
    virtual void update(const float &deltaTime) noexcept override=0;
};

class NullSoundEngine: public SoundEngine{
  private:
    void update(const float &deltaTime) noexcept override final{}
    void injectDependency() noexcept override final{}
};
