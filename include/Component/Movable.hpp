#pragma once

#include "Component.hpp"

namespace Game{
    namespace Component{
        class Movable final: public Interface{
        public:
            Movable() noexcept: Interface(201){}
            ~Movable()=default;

        private:
            void postConstruct() noexcept override final;
            void update(const Time& deltaTime) noexcept override final{}

            Type getType() const noexcept override final{
                return Type::Movable;
            }
            std::weak_ptr<Skin::Attribute_2D> get(){ return attr; }

        public:
            std::shared_ptr<Skin::Attribute_2D> attr;
        };
    }
}
