#pragma once

#include <concepts>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "Makable.hpp"
#include "Math.hpp"
#include "Observable.hpp"
#include "Observer.hpp"

class SubEngine;
class Component;
enum class ComponentName;

// Actor interface

class Actor : public std::enable_shared_from_this<Actor>, public Makable<Actor, SubEngine>, public Observer<Component>, public Observable<Actor> {
    friend class Component;

  public:
    // 액터의 상태를 추적하는 데 사용
    enum State {
        EActive,
        EPaused,
        EDead
    };

  public:
    virtual ~Actor() = default;

    // Game에서 input을 받는 함수
    void processInput(const uint8_t *keyState) noexcept;
    // 특정 액터를 위한 입력 코드
    virtual void processActorInput(const uint8_t *keyState) {}
    // Game에서 호출하는 함수
    void update(const float &deltaTime) noexcept;
    // 특정 액터에 특화된 업데이트 코드
    virtual void updateActor(const float &deltaTime) noexcept {}

    // Getters/Setters
    const State &getState() const noexcept { return state; }
    const std::weak_ptr<SubEngine> &getActorManager() const noexcept { return owner; }
    Vector2 getSize() const noexcept { return scale * baseSize; }

    void onNotify(std::shared_ptr<Component> comp, Observable_msg msg);
    void appendComponent(const std::shared_ptr<Component> component) noexcept;
    std::weak_ptr<Component> queryComponent(const ComponentName name) noexcept;

  protected:
    Actor(const std::weak_ptr<SubEngine> manager) noexcept;
    virtual void postConstruct() noexcept {}

  private:
    void updateComponents(const float &deltaTime) noexcept;
    void orderComponents() noexcept;

  public:
    Vector2 position;
    Vector2 baseSize;
    Math::Real scale = 1.0;
    Math::Radian rotation = 0.0;

  protected:
    const std::weak_ptr<SubEngine> owner;
    State state = EActive;
    // 액터 구현체가 보유한 컴포넌트들
    std::map<ComponentName, std::shared_ptr<Component>> components;
    struct comp_update_order {
        using ptr = std::shared_ptr<Component>;
        bool operator()(const ptr &lhs, const ptr &rhs) {
            return lhs->getUpdateOrder() < rhs->getUpdateOrder();
        }
    };
    std::multiset<std::shared_ptr<Component>, comp_update_order> orderedComponents;
};

// Concrete Actor

class Paddle : public Actor {
  public:
    void updateActor(const float &deltaTime) noexcept override;

    void allowCollision(const std::weak_ptr<Actor> opponent) noexcept;

  protected:
    Paddle(const std::weak_ptr<SubEngine> owner) noexcept;

  private:
    void postConstruct() noexcept override;

  private:
    std::shared_ptr<class BoxComponent> bc;
    std::shared_ptr<class CollisionComponent> cc;
    std::shared_ptr<class InputComponent> ic;
    std::shared_ptr<class MoveComponent> mc;
};

class Wall : public Actor {
  private:
    Wall(const std::weak_ptr<SubEngine> owner) noexcept;

  protected:
    void postConstruct() noexcept override;

  private:
    std::shared_ptr<class BoxComponent> bc;
    std::shared_ptr<class MoveComponent> mc;
};

class Ball : public Actor {
  public:
    void updateActor(const float &deltaTime) noexcept override;
    void allowCollision(const std::weak_ptr<Actor> opponent) noexcept;

  protected:
    Ball(const std::weak_ptr<SubEngine> owner) noexcept;

  private:
    void postConstruct() noexcept override;

  private:
    std::shared_ptr<class AnimSpriteComponent> sc;
    std::shared_ptr<class CollisionComponent> cc;
    std::shared_ptr<class MoveComponent> mc;
};

class Ship : public Actor {
  public:
    void updateActor(const float &deltaTime) noexcept override;

  protected:
    Ship(const std::weak_ptr<SubEngine> owner) noexcept;

  private:
    void postConstruct() noexcept override;

  private:
    std::shared_ptr<class AnimSpriteComponent> sc;
    std::shared_ptr<class InputComponent> ic;
    std::shared_ptr<class MoveComponent> mc;
};

class Asteroid : public Actor {
  public:
    Asteroid(const std::weak_ptr<SubEngine> owner) noexcept;

  private:
    void postConstruct() noexcept override;

  private:
    std::shared_ptr<class SpriteComponent> sc;
    std::shared_ptr<class MoveComponent> mc;
};
