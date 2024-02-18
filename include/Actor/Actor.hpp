#pragma once

#include <map>
#include <optional>
#include <set>

#include "Makable.hpp"

#include "gefwd.hpp"

// 액터의 상태를 추적하는 데 사용
enum class State{
    EActive,
    EPaused,
    EDead
};

namespace Game{
    namespace Actor{
        class Actor: public Makable<Actor, SubEngine::ActorManager>{
        public:
            Actor() noexcept=default;
            virtual ~Actor()=default;

            void update(const Time& deltaTime) noexcept;
            const State& getState() const noexcept{ return state; }

            std::shared_ptr<Component::Interface>
                find(const Type name) noexcept;
            std::optional<std::shared_ptr<SubEngine::Interface>>
                query(const SubEngine::Type name) noexcept;

        private:
            // actor-specific function
            virtual void updateActor(const float& deltaTime) noexcept=0;

        protected:
            State state;

            struct UpdateOrder {
                using ptr=std::shared_ptr<Component::Interface>;
                bool operator()(const ptr& lhs, const ptr& rhs) const noexcept;
            };
            // the component owned by actor
            std::multimap<Type,
                std::shared_ptr<Component::Interface>
            > components;
            std::multiset<std::shared_ptr<Component::Interface>, UpdateOrder
            > orderedComponents;
        };

        class NullActor final: public Actor{
        private:
            void updateActor(const float& deltaTime) noexcept override final{}
        };
    }
}
