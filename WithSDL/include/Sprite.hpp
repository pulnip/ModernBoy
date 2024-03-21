#pragma once

#include <SDL2/SDL_render.h>
#include "Skin.hpp"
#include "Engine/Logger.hpp"
#include "Component/Drawable.hpp"

namespace WithSDL{
    // 단일 스프라이트 텍스처
    class Sprite: public Component::Drawable{
    public:
        Sprite(
            std::weak_ptr<Actor::Vanilla> actor,
            int drawOrder
        ) noexcept;
        virtual ~Sprite()=default;

        void setTexture(SDL_Texture* t) noexcept{ texture=t; }

    private:
        virtual void update(const Game::Time&) noexcept override{}
        virtual void draw() noexcept override;

    private:
        Logging::Bind logger={"Sprite", id};
        SDL_Texture* texture;
    };
}
