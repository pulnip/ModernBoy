#pragma once

#include "Actor/Actor.hpp"

class Asteroid : public Actor{
  public:
    ~Asteroid()=default;

  protected:
    Asteroid() noexcept=default;

  private:
    void updateActor(const float&) noexcept override final{}
    void injectDependency() noexcept override;
};
