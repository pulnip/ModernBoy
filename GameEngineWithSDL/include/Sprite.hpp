#pragma once

#include "Observable.hpp"

#include "Skin.hpp"
#include "Component/Drawable.hpp"

namespace WithSDL{
    namespace Component{
        // 단일 스프라이트 텍스처
        class Sprite: public Game::Component::Drawable{
        public:
            Sprite(int drawOrder=201) noexcept: Drawable(drawOrder){}
            virtual ~Sprite()=default;

            void setCanvas() noexcept override final;
            void setTexture(SDL_Texture* t) noexcept{ texture=t; }

        private:
            virtual void tryDraw() noexcept override;

        private:
            SDL_Texture* texture;
        };
    }
}
