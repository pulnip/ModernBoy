#pragma once

#include "Component.hpp"

class AIComponent: public Component{
  private:
    enum State {
        Death,
        Patrol,
        Attack
    };
  public:
    virtual ~AIComponent()=default;

  protected:
    AIComponent() noexcept=default;
    
    void injectDependency() noexcept override{}

  private:
    ComponentName getName() const noexcept override final{
        return ComponentName::AIComponent;
    }

    int initUpdateOrder() const noexcept override final{ return 150; }

  private:
    void update(const float &deltaTime) noexcept override=0;
};
