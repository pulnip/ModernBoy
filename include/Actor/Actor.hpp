#pragma once

#include <map>
#include <set>

#include "Ownership.hpp"
#include "IActor.hpp"

class Actor: public IActor,
    public Owned<Actor, ActorManager>,
    public Owner<Component>
{
  public:
    virtual ~Actor()=default;

    template<typename Concrete>
        requires std::derived_from<Concrete, Actor>
    static std::shared_ptr<IActor>
    create(const std::weak_ptr<ActorManager> manager) noexcept{
        return make<Concrete>(manager);
    }

  protected:
    Actor() noexcept=default;

    std::shared_ptr<IComponent>
    find(const ComponentName name) noexcept override final;
    std::optional<std::shared_ptr<ISubEngine>>
    query(const SubEngineName name) noexcept override final;

  private:
    void update(const float& deltaTime) noexcept override final;
    void setProperty() noexcept override final{ injectDependency(); }
    void onNotify(Lifetime msg, std::shared_ptr<Component> c) noexcept override final;

  private:
    // actor-specific function
    virtual void updateActor(const float& deltaTime) noexcept=0;
    virtual void injectDependency() noexcept=0;

  protected:
    struct UpdateOrder {
        using ptr=std::shared_ptr<IComponent>;
        bool operator()(const ptr& lhs, const ptr& rhs) const noexcept;
    };
    // the component owned by actor
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
