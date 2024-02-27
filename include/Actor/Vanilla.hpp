#pragma once

#include <map>
#include <optional>
#include <set>
#include "Engine/Logger.hpp"
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
        virtual ~Vanilla()=default;

        virtual void initAbility() noexcept=0;
        void update(const Game::Time&) noexcept;

        void add(pComponent) noexcept;
        std::optional<pComponent> get(Component::Type) noexcept;
        void remove(Component::Type) noexcept;

        State getState() noexcept{ return state; }

        virtual void updateActor(const Game::Time&) noexcept=0;

      protected:
        const int id=++seed;
        State state=State::Active;
        std::multimap<Component::Type, pComponent> components;

      private:
        ::Logger::Binded logger={"Actor", id};
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