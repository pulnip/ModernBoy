#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include "Observer.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"
#include "Component/Ability.hpp"

namespace Component{
    class Controllable final: public Ability,
        public Observer<::Skin::Key>,
        public std::enable_shared_from_this<Controllable>
    {
      public:
        Controllable(std::weak_ptr<Actor::Vanilla> actor) noexcept;

        void update(const Game::Time&) noexcept override final{}

        void setKey(
            const uint8_t key,
            std::shared_ptr<Callable> OnPressed=doNothing,
            std::shared_ptr<Callable> OnReleased=doNothing
        ) noexcept;

      private:
        void onNotify(const ::Skin::Key& key) noexcept override final;

        Type getType() noexcept override final{
            return Type::Controllable;
        }

      protected:
        std::map<uint8_t, std::shared_ptr<Callable>> ifPressed;
        std::map<uint8_t, std::shared_ptr<Callable>> ifReleased;
    
      private:
        ::Logger::Binded logger={"Controllable", id};
    };

    namespace Behaviour{
        template<typename TargetComponent>
        class Base: public Connectable{
          public:
            Base(std::weak_ptr<Actor::Vanilla> actor) noexcept: actor(actor){}

          protected:
            std::optional<std::shared_ptr<TargetComponent>> getTarget() noexcept;

          private:
            void connect() noexcept override final;
        
          private:
            std::weak_ptr<Actor::Vanilla> actor;
            std::weak_ptr<TargetComponent> target;
        };
        class MoveX: public Base<Movable>, public Callable{
          public:
            MoveX(std::weak_ptr<Actor::Vanilla> actor, double velocity) noexcept:
            Base(actor), vx(velocity){}

            void operator()() noexcept override;

          private:
            double vx;
        };
        class MoveY: public Base<Movable>, public Callable{
          public:
            MoveY(std::weak_ptr<Actor::Vanilla> actor, double velocity) noexcept:
            Base(actor), vy(velocity){}

            void operator()() noexcept override;

          private:
            double vy;
        };
    }
}
