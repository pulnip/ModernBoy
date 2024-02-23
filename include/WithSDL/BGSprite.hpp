#pragma once

#include <vector>

#include "Math.hpp"
#include "WithSDL/Sprite.hpp"

using namespace Game;
using namespace My::Math;

namespace WithSDL{
    // 스크롤되는 배경
    class BGSprite final: public Sprite{
    public:
        BGSprite(
            std::weak_ptr<Actor::Vanilla> actor
        ) noexcept;
        ~BGSprite();

        // 배경용 텍스처 설정
        void setBGTextures(
            const std::vector<class SDL_Texture*> &textures
        ) noexcept;
        void setScreenSize(const Vector2D& size) noexcept{
            screenSize = size;
        }
        Number::Real getScrollSpeed() const noexcept{
            return scrollSpeed;
        }
        void setScrollSpeed(const Number::Real speed) noexcept{
            scrollSpeed = speed;
        }

    private:
        void draw() noexcept override final;
        void update(const Time& deltaTime) noexcept override final;

    private:
        std::unique_ptr<Engine::BindedLogger> logger;
        struct BGTexture {
            class SDL_Texture* texture;
            // 화면이 시작하는 위치
            Number::Real offset_x;
        };
        std::vector<BGTexture> BGTextures;
        Vector2D screenSize;
        Time scrollSpeed = 0.0f;
    };
}
