#pragma once

#include <memory>

#include "Skin.hpp"
#include "Component/Ability.hpp"

namespace Component{
    class Drawable: public Ability{
      public:
        Drawable(
            std::weak_ptr<Actor::Vanilla> actor, int updateOrder,
            int drawOrder
        ) noexcept;
        virtual ~Drawable();

        virtual void update(const Game::Time&) noexcept override=0;
        virtual void draw() noexcept=0;

        int getDrawOrder() const noexcept{ return drawOrder; }

      protected:
        std::weak_ptr<Movable> target;
        /* 그리기 순서(화가 알고리즘 *drawOrder이 작을수록 더 뒤에 위치)
        - 배경 계열: 100 to 199
        - 일반 오브젝트 계열: 200 to 299
        - player계열: 300 to 399
        */
        int drawOrder=0;
      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };

    class Colored final: public Drawable{
      public:
        Colored(
            std::weak_ptr<Actor::Vanilla>, int updateOrder,
            int drawOrder,
            Skin::TrueColor
        ) noexcept;
        ~Colored();

        void update(const Game::Time&) noexcept override final{}
        void draw() noexcept override final;
    
      private:
        std::unique_ptr<Engine::BindedLogger> logger;
        Skin::TrueColor color;
    };
}