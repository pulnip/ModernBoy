#pragma once

#include "Makable.hpp"
#include "Observable.hpp"
#include "gefwd.hpp"
#include "IComponent.hpp"

class Component: public IComponent,
    public Makable<Component, IActor>,
    public Observable<Lifetime, IComponent>
{
  public:
    virtual ~Component() = default;

  protected:
    Component() noexcept=default;
    void postConstruct() noexcept override final;

  private:
    int getUpdateOrder() const noexcept override final { return updateOrder; }

  private:
    virtual void injectDependency() noexcept=0;

  protected:
    // 컴포넌트의 업데이트 순서
    // updateOrder이 작을수록 더 빨리 갱신
    // input 계열: 100 to 199
    // 연산 계열: 200 to 299
    // output계열: 300 to 399
    int updateOrder = 0;
};

enum class ComponentName {
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

class NullComponent: public Component{
  private:
    void update(const float& deltaTime) noexcept override{}
    ComponentName getName() const noexcept override{
        return ComponentName::NullComponent;
    }
};
