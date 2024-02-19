#pragma once

#include "Makable.hpp"

#include "gefwd.hpp"

namespace Game{
    namespace Component{
        enum class Type{
            Interface,
            AI,
            Rigidbody,
            Drawable,
            Controllable,
            Movable
        };

        class Interface: public Makable<Interface, Actor::Interface>,
            public std::enable_shared_from_this<Interface>
        {
        public:
            Interface(int updateOrder=0
            ) noexcept: updateOrder(updateOrder){}
            virtual ~Interface()=default;

            virtual void update(const Time& deltaTime) noexcept=0;

            virtual Type getType() const noexcept{
                return Type::Interface;
            }
            const int& getUpdateOrder() const noexcept{
                return updateOrder;
            }
            void setUpdateOrder(const int uo) noexcept{
                updateOrder=uo;
            }

        protected:
            void postConstruct() noexcept override;

        protected:
            /* 컴포넌트의 업데이트 순서
            - updateOrder이 작을수록 더 빨리 갱신
            - input 계열: 100 to 199
            - 연산 계열: 200 to 299
            - output계열: 300 to 399
            */
            int updateOrder=0;
        };
    }
}
namespace Null{
    namespace Component{
        class Component final: public Game::Component::Interface{
        private:
            void update(const Game::Time& deltaTime
            ) noexcept override final{}
        };
    }
}
