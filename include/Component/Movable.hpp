#pragma once

#include <memory>

#include "Skin.hpp"
#include "Engine/Logger.hpp"
#include "Component/Ability.hpp"

namespace Component{
    class Movable final: public Ability{
      public:
        Movable(
            std::weak_ptr<Actor::Vanilla> actor
        ) noexcept;

        void update(const Game::Time&) noexcept override final;
        Type getType() noexcept{
            return Type::Movable;
        }
        auto& get(){ return attr; }

      private:
        ::Logger::Binded logger={"Movable", id};
        ::Skin::Attribute_2D attr;
    };
}