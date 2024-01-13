#pragma once

#include <memory>

#include "Makable.hpp"
#include "Observable.hpp"

enum class Lifetime;
class Actor;

enum class ComponentName {
    Component,

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

class Component : public Makable<Component, Actor>, public Observable<Lifetime, Component> {
  public:
    virtual ~Component() = default;

    virtual void processInput(const uint8_t *keyState) {}
    virtual void update(const float &deltaTime) noexcept = 0;

    virtual ComponentName getName() const noexcept = 0;
    int getUpdateOrder() const noexcept { return updateOrder; }

  protected:
    Component(const std::weak_ptr<Actor> owner) noexcept;
    virtual void postConstruct() noexcept override;

  protected:
    const std::weak_ptr<Actor> owner;
    // 컴포넌트의 업데이트 순서
    // updateOrder이 작을수록 더 빨리 갱신
    // input 계열: 100 to 199
    // 연산 계열: 200 to 299
    // output계열: 300 to 399
    int updateOrder = 0;
};
