#pragma once

#include <SDL2/SDL_render.h>
#include "Skin.hpp"
#include "Component/Drawable.hpp"

namespace WithSDL{
    // 단일 스프라이트 텍스처
    class Sprite: public Component::Drawable{
    public:
        Sprite(
            std::weak_ptr<Actor::Vanilla> actor,
            int drawOrder=200
        ) noexcept;
        virtual ~Sprite();

        void setTexture(SDL_Texture* t) noexcept{ texture=t; }

    private:
        virtual void draw() noexcept override;

    private:
        std::unique_ptr<Engine::BindedLogger> logger;
        SDL_Texture* texture;
    };
}
