#pragma once

#include "Ownership.hpp"
#include "gefwd.hpp"
#include "IComponent.hpp"

class Component: public IComponent,
    public Owned<Component, Actor>
{
  public:
    virtual ~Component()=default;

  protected:
    Component() noexcept=default;

  protected:
    virtual void setProperty() noexcept override final{
        #warning "delete it"
        updateOrder=initUpdateOrder();
        injectDependency();
    }

  private:
    virtual void update(const float& deltaTime) noexcept override=0;

    virtual int initUpdateOrder() const noexcept=0;
    virtual ComponentName getName() const noexcept{
        return ComponentName::Component;
    }
    virtual void injectDependency() noexcept=0;
};

class NullComponent final: public Component{
  public:
    NullComponent() noexcept=default;
    ~NullComponent()=default;

  private:
    void update(const float& deltaTime) noexcept override final{}

    void injectDependency() noexcept override final{}

    int initUpdateOrder() const noexcept override final{
        return 0;
    }
};
