#pragma once

#include <vector>

#include "Sprite.hpp"

namespace WithSDL{
    namespace Component{
        class AnimSprite final: public Sprite{
        public:
            AnimSprite() noexcept: Sprite(202){}
            ~AnimSprite()=default;

            // 애니메이션 FPS 세팅
            void setAnimFPS(const float fps) noexcept{ animFPS = fps; }
            // 애니메이션에 사용되는 텍스처 설정
            void setAnimTextures(
                const std::vector<class SDL_Texture *> &textures
            ) noexcept{
                animTextures = textures;
            }

        private:
            // 애니메이션을 프레임마다 갱신
            void update(const Game::Time& deltaTime
            ) noexcept override final;

        private:
            std::vector<class SDL_Texture *> animTextures;
            // 현재 프레임
            float currFrame = 0.0f;
            int animFPS = 8;
        };
    }
}

