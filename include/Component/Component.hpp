#pragma once

#include "Observable.hpp"
#include "IComponent.hpp"

enum class ComponentName{
    Component,
    NullComponent,

    CollisionComponent,

    DrawComponent,
    BoxComponent,
    SpriteComponent,
    AnimSpriteComponent,
    BGSpriteComponent,

    MoveComponent,

    InputComponent,

    AIComponent
};

class Component: public IComponent,
    public Observable<Lifetime, IComponent>
{
  public:
    virtual ~Component();

  protected:
    Component() noexcept=default;

  private:
    void postConstruct() noexcept override final;

  private:
    virtual ComponentName getName() const noexcept{
        return ComponentName::Component;
    }
    virtual void injectDependency() noexcept=0;
    virtual void update(const float& deltaTime) noexcept override=0;

    virtual int initUpdateOrder() const noexcept=0;
};

class NullComponent: public Component{
  private:
    void injectDependency() noexcept override final{}
    ComponentName getName() const noexcept override final{
        return ComponentName::NullComponent;
    }
    void update(const float& deltaTime) noexcept override final{}

    int initUpdateOrder() const noexcept override final{
        return 0;
    }
};
