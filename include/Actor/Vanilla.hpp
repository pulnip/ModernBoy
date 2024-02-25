#pragma once

#include <map>
#include <optional>
#include <set>

#include "myfwd.hpp"

namespace Actor{
    enum class Role{
        Vanilla
    };

    enum class State{
        Active, Paused, Dead
    };

    class Vanilla: public std::enable_shared_from_this<Vanilla>{
      public:
        Vanilla() noexcept;
        virtual ~Vanilla();

        virtual void initAbility() noexcept=0;
        void update(const Game::Time&) noexcept;

        void add(pComponent) noexcept;
        std::optional<pComponent> get(Component::Type) noexcept;
        void remove(Component::Type) noexcept;

        State getState() noexcept{ return state; }

        virtual void updateActor(const Game::Time&) noexcept=0;

      protected:
        const int id;
        State state=State::Active;
        std::multimap<Component::Type, pComponent> components;

      private:
        std::unique_ptr<Engine::BindedLogger> logger;

        static int seed;
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