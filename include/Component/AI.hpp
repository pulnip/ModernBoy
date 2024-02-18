#pragma once

#include "Component.hpp"

namespace Game{
    namespace Component{
        class AI: public Interface{
        private:
            enum State {
                Death,
                Patrol,
                Attack
            };
        public:
            AI() noexcept: Interface(150){}
            virtual ~AI()=default;

        private:
            Type getType() const noexcept override final{
                return Type::AI;
            }
        };
    }
}
namespace Null{
    namespace Component{
        class AI final: public Game::Component::AI{
        private:
            void update(const Game::Time& deltaTime
            ) noexcept override final{}
        };
    }
}
