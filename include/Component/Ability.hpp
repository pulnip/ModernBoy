#pragma once

#include <memory>
#include "myfwd.hpp"

namespace Component{
    enum class Type{
        Base,
        AI,
        Controllable,
        Drawable,
        Movable,
        Rigidbody
    };

    class Ability{
      public:
        Ability(
            std::weak_ptr<Actor::Vanilla> actor, int updateOrder
        ) noexcept;
        virtual ~Ability();

        virtual void update(const Game::Time&) noexcept=0;

        virtual Type getType() noexcept{
            return Type::Base;
        }
        int getUpdateOrder() const noexcept{
            return updateOrder;
        }

      protected:
        const int id;
        std::weak_ptr<Actor::Vanilla> actor;
        int updateOrder;
      private:
        static int seed;
        std::unique_ptr<Engine::BindedLogger> logger;
    };

    class Null final: public Ability{
        void update(const Game::Time&) noexcept override final{}
    };
}