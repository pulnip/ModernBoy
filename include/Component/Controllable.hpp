#pragma once

#include <cstdint>
#include <functional>
#include <map>

#include "Component.hpp"

namespace Game{
    namespace Component{
        class Controllable final: public Interface,
            public Observer<Key>
        {
        public:
            Controllable() noexcept: Interface(100){}
            ~Controllable()=default;

            void setKey(
                const uint8_t key,
                std::function<void(void)> OnPressed,
                std::function<void(void)> OnReleased=[](){}
            ) noexcept;

        private:
            void update(const Time& deltaTime) noexcept override final{}
            void onNotify(const Key& key) noexcept override final;

            Type getType() const noexcept override final{
                return Type::Controllable;
            }
            void setController() noexcept;

        protected:
            std::map<uint8_t, std::function<void(void)>> ifPressed;
            std::map<uint8_t, std::function<void(void)>> ifReleased;

        private:
            std::weak_ptr<InputSystem> controller;
        };
    }
}