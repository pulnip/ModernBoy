#pragma once

#include <cstdint>
#include <functional>
#include <map>
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
            std::function<void(void)> OnPressed,
            std::function<void(void)> OnReleased=[](){}
        ) noexcept;

      private:
        void onNotify(const ::Skin::Key& key) noexcept override final;

        Type getType() noexcept override final{
            return Type::Controllable;
        }

      protected:
        std::map<uint8_t, std::function<void(void)>> ifPressed;
        std::map<uint8_t, std::function<void(void)>> ifReleased;
    
      private:
        ::Logger::Binded logger={"Controllable", id};
    };
}
