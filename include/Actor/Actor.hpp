#pragma once

#include <map>
#include <set>

#include "Makable.hpp"
#include "Observable.hpp"
#include "gefwd.hpp"
#include "IActor.hpp"

class Actor: public IActor,
    public Makable<Actor, ActorManager>,
    public Observable<Lifetime, IActor>
{
  public:
    virtual ~Actor() = default;

  protected:
    Actor() noexcept=default;

    std::optional<std::weak_ptr<IComponent>>
    queryComponent(const ComponentName name) noexcept override final;
    const std::weak_ptr<ActorManager>&
    getActorManager() const noexcept override final{ return owner; }

  private:
    void postConstruct() noexcept override final;

    void update(const float& deltaTime) noexcept override final;
    const State& getState() const noexcept override final{ return state; }
    void onNotify(MSG_t msg, spObservable comp) noexcept override final;

  private:
    // 특정 액터에 특화된 업데이트 코드
    virtual void updateActor(const float& deltaTime) noexcept = 0;
    virtual void injectDependency() noexcept=0;

  protected:
    State state = State::EActive;
    struct UpdateOrder {
        using ptr = std::shared_ptr<IComponent>;
        bool operator()(const ptr& lhs, const ptr& rhs) const noexcept;
    };
    // 액터 구현체가 보유한 컴포넌트들
    std::map<ComponentName, std::shared_ptr<IComponent>> components;
    std::multiset<std::shared_ptr<IComponent>, UpdateOrder> orderedComponents;
};

class NullActor final: public Actor{
  protected:
    NullActor() noexcept=default;

  private:
    void updateActor(const float& deltaTime) noexcept override final{}
    void injectDependency() noexcept override final{}
};
