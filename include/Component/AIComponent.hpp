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

  private:
    ComponentName getName() const noexcept override final{
        return ComponentName::AIComponent;
    }
    int initUpdateOrder() const noexcept override final{ return 150; }

  protected:
    virtual void injectDependency() noexcept override{}

  private:
    virtual void update(const float &deltaTime) noexcept override=0;
};

class NullAIComponent final: public AIComponent{
public:
    NullAIComponent() noexcept=default;
    ~NullAIComponent()=default;

private:
    void update(const float& deltaTime) noexcept override final{}
};
