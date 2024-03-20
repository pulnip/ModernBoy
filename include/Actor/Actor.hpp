#pragma once

#include <map>
#include <optional>
#include <set>
#include "Makable.hpp"
#include "FWD.hpp"
#include "Engine/Types.hpp"
#include "System/Logging/Bind.hpp"
#include "Actor/Traits.hpp"
#include "Component/Traits.hpp"

class MSG_Q{

};

class Actor{
    using pComp=std::shared_ptr<Component>;
    using Type=ComponentTraits::Type;
    using State=ActorTraits::State;
    using Role=ActorTraits::Role;

  public:
    virtual ~Actor()=default;

    void update(const Game::Time&) noexcept;

    void append(pComp) noexcept;
    std::optional<pComp> get(Type) noexcept;
    void remove(Type) noexcept;

    State getState() noexcept{ return state; }
    Role getRole() noexcept{ return role; }
    void setRole(const Role& r) noexcept{ role=r; }
    virtual void updateActor(const Game::Time&) noexcept{};

  public:
    const int id=++seed;

  private:
    static int seed;
    Logging::Bind logger={"Actor", std::to_string(id)};
    
    State state=State::Active;
    Role role;
    
    std::multimap<Type, pComp> components;
    struct UpdateOrder{
        bool operator()(
            const pComp& lhs, const pComp& rhs
        ) const noexcept;
    };
    std::multiset<pComp, UpdateOrder> ordered;
};

class NullActor final: public Actor{
    void updateActor(const Game::Time&) noexcept override final{}
};
