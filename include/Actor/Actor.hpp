#pragma once

#include <map>
#include <set>

#include "Observable.hpp"
#include "IActor.hpp"

class Actor: public IActor,
    public Observable<Lifetime, IActor>
{
  public:
    virtual ~Actor();

  protected:
    Actor() noexcept=default;

  private:
    void postConstruct() noexcept override final;
    void onNotify(Lifetime msg, std::shared_ptr<IComponent> c) noexcept override final;

    void update(const float& deltaTime) noexcept override final;
    std::shared_ptr<IComponent>
    find(const ComponentName name) noexcept override final;
    std::optional<std::shared_ptr<ISubEngine>>
    query(const SubEngineName name) noexcept override final;

  private:
    // 특정 액터에 특화된 업데이트 코드
    virtual void injectDependency() noexcept=0;
    virtual void updateActor(const float& deltaTime) noexcept=0;

  protected:
    struct UpdateOrder {
        using ptr = std::shared_ptr<IComponent>;
        bool operator()(const ptr& lhs, const ptr& rhs) const noexcept;
    };
    // 액터 구현체가 보유한 컴포넌트들
    std::map<ComponentName, std::shared_ptr<IComponent>> components;
    std::multiset<std::shared_ptr<IComponent>, UpdateOrder> orderedComponents;
};

class NullActor final: public Actor{
  public:
    ~NullActor()=default;

  protected:
    NullActor() noexcept=default;

  private:
    void injectDependency() noexcept override final{}
    void updateActor(const float& deltaTime) noexcept override final{}
};
