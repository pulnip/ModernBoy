#pragma once

#include "SubEngine.hpp"

class PhysicsSimulator: public SubEngine,
    public Observer<Attribute_2D>
{
  public:
    virtual ~PhysicsSimulator()=default;

  protected:
    PhysicsSimulator() noexcept=default;

  private:
    void injectDependency() noexcept override final{}
    SubEngineName getName() const noexcept override{
        return SubEngineName::PhysicsSimulator;
    }

  private:
    virtual void update(const float &deltaTime) noexcept override=0;
    virtual void onNotify(Attribute_2D attr) noexcept override=0;
};

class NullPhysicsSimulator: public PhysicsSimulator{
  private:
    void update(const float &deltaTime) noexcept override final{}
    void onNotify(Attribute_2D attr) noexcept override final;
};

// for Leges motus Newtoni
class PhysicsSimulator_default: public PhysicsSimulator{
  private:
    void update(const float &deltaTime) noexcept override final;
    void onNotify(Attribute_2D attr) noexcept override final;
};
