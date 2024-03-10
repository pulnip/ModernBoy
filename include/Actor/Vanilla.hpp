#pragma once

#include <map>
#include <optional>
#include <set>
#include "Blueprint.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Actor{
    enum class Role{
        Neutral, Player, Enemy
    };

    enum class State{
        Active, Paused, Dead
    };

    class Vanilla: public Makable<Vanilla, false, false>,
        public std::enable_shared_from_this<Vanilla>
    {
      public:
        virtual ~Vanilla()=default;

        virtual void initAbility() noexcept{};
        void update(const Game::Time&) noexcept;

        void add(pComponent) noexcept;
        std::optional<pComponent> get(Component::Type) noexcept;
        void remove(Component::Type) noexcept;

        State getState() noexcept{ return state; }
        Role getRole() noexcept{ return role; }
        void setRole(const Role& r) noexcept{ role=r; }

        virtual void updateActor(const Game::Time&) noexcept{};

      protected:
        const int id=++seed;
        State state=State::Active;
        std::multimap<Component::Type, pComponent> components;

      private:
        ::Logger::Binded logger={"Actor", id};
        static int seed;
        Role role;
        struct UpdateOrder{
            bool operator()(
                const pComponent& lhs, const pComponent& rhs
            ) const noexcept;
        };
        std::multiset<pComponent, UpdateOrder> ordered;
    };

    class Null final: public Vanilla{
        void updateActor(const Game::Time&) noexcept override final{}
    };
}