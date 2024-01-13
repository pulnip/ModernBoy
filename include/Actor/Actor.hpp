#pragma once

#include <map>
#include <memory>
#include <set>

#include "Makable.hpp"
#include "Math.hpp"
#include "Observable.hpp"
#include "Observer.hpp"

enum class Lifetime;
class ActorManager;
class Component;
enum class ComponentName;

class Actor : public Makable<Actor, ActorManager>, public Observer<Lifetime, Component>, public Observable<Lifetime, Actor> {
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
    const std::weak_ptr<ActorManager> &getActorManager() const noexcept { return owner; }
    Vector2 getSize() const noexcept { return scale * baseSize; }

    void onNotify(MSG_t msg, spObservable comp) noexcept override;
    std::optional<std::weak_ptr<Component>>
    queryComponent(const ComponentName name) noexcept;

  protected:
    Actor(const std::weak_ptr<ActorManager> manager) noexcept;
    virtual void postConstruct() noexcept override;

  private:
    void appendComponent(const std::shared_ptr<Component> component) noexcept;
    void updateComponents(const float &deltaTime) noexcept;

  public:
    Vector2 position;
    Vector2 baseSize;
    Math::Real scale = 1.0;
    Math::Radian rotation = 0.0;

  protected:
    const std::weak_ptr<ActorManager> owner;
    State state = EActive;
    // 액터 구현체가 보유한 컴포넌트들
    std::map<ComponentName, std::shared_ptr<Component>> components;
    struct UpdateOrder {
        using ptr = std::shared_ptr<Component>;
        bool operator()(const ptr &lhs, const ptr &rhs) const;
    };
    std::multiset<std::shared_ptr<Component>, UpdateOrder> orderedComponents;
};