#pragma once

#include "Observer.hpp"
#include "SubEngine.hpp"

struct Matter;

class PhysicsSimulator : public SubEngine, public Observer<Matter> {
  public:
    void update(const float &deltaTime) noexcept override;

  protected:
    PhysicsSimulator() noexcept=default;

  private:
    void postConstruct() noexcept override;
    void onNotify(Matter matter) noexcept override;
};
