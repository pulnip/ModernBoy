#pragma once

#include <memory>

#include "Skin.hpp"
#include "Component/Ability.hpp"

namespace Component{
    class Movable final: public Ability{
      public:
        Movable(
            std::weak_ptr<Actor::Vanilla> actor, int updateOrder=200
        ) noexcept;
        ~Movable();

        void update(const Game::Time&) noexcept override final;
        Type getType() noexcept{
            return Type::Movable;
        }
        auto& get(){ return attr; }

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
        ::Skin::Attribute_2D attr;
    };
}