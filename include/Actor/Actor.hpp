#pragma once

#include <map>
#include <optional>
#include <set>

#include "Makable.hpp"

#include "gefwd.hpp"

namespace Game{
    namespace Actor{
        enum class Type{
            Interface,
            Player,
            Enemy
        };
        // 액터의 상태를 추적하는 데 사용
        enum class State{
            EActive,
            EPaused,
            EDead
        };
        class Interface:
            public Makable<Interface, SubEngine::ActorManager>,
            public std::enable_shared_from_this<Interface>
        {
        public:
            Interface() noexcept=default;
            virtual ~Interface()=default;

            void update(const Time& deltaTime) noexcept;
            Part find(const Component::Type name) noexcept;
            std::optional<Plugin> query(
                const SubEngine::Type name
            ) noexcept;
            
            const State& getState() const noexcept{ return state; }

            void add(Part comp) noexcept;
            void remove(Part comp) noexcept;

        protected:
            void postConstruct() noexcept override;

        private:
            // actor-specific function
            virtual void updateActor(const Time& deltaTime) noexcept=0;

        protected:
            State state;

            struct UpdateOrder {
                bool operator()(
                    const Part& lhs, const Part& rhs
                ) const noexcept;
            };
            // the component owned by actor
            std::multimap<Component::Type, Part> components;
            std::multiset<Part, UpdateOrder> orderedComponents;
        };
    }
}

namespace Null{
    namespace Actor{
        class Dummy final: public Game::Actor::Interface{
        private:
            void updateActor(const Game::Time& deltaTime
            ) noexcept override final{}
        };
    }
}
