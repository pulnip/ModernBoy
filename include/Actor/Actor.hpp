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

  private:
    // 특정 액터에 특화된 업데이트 코드
    virtual void updateActor(const float& deltaTime) noexcept = 0;

  protected:
    Actor() noexcept=default;

    // Should Move to MoveComponent
    const Vector2& getPosition() const noexcept override final{ return position; }
    void setPosition(const Vector2 pos) noexcept final{ position = pos; }
    void setBaseSize(const Vector2& bsize) noexcept final{ baseSize = bsize; }
    Vector2 getSize() const noexcept override final{ return scale * baseSize; }
    const Math::Real& getScale() const noexcept override final{ return scale; }
    const Math::Real& getRotation() const noexcept override final{ return rotation; }
    void setRotation(const Math::Real& rot) noexcept override final{ rotation = rot; }

  private:
    void postConstruct() noexcept override final;
    virtual void injectDependency() noexcept=0;

    void update(const float& deltaTime) noexcept override final;
    const State& getState() const noexcept override final{ return state; }
    const std::weak_ptr<ActorManager>&
    getActorManager() const noexcept override final{ return owner; }
    std::optional<std::weak_ptr<IComponent>>
    queryComponent(const ComponentName name) noexcept override final;
    void onNotify(MSG_t msg, spObservable comp) noexcept override final;

  protected:
    State state = State::EActive;
    struct UpdateOrder {
        using ptr = std::shared_ptr<IComponent>;
        bool operator()(const ptr& lhs, const ptr& rhs) const noexcept;
    };
    // 액터 구현체가 보유한 컴포넌트들
    std::map<ComponentName, std::shared_ptr<IComponent>> components;
    std::multiset<std::shared_ptr<IComponent>, UpdateOrder> orderedComponents;

    Vector2 position;
    Vector2 baseSize;
    Math::Real scale = 1.0;
    Math::Radian rotation = 0.0;
};

class NullActor final: public Actor{
  protected:
    NullActor() noexcept=default;

  private:
    void updateActor(const float& deltaTime) noexcept override final{}
    void injectDependency() noexcept override final{}
};
