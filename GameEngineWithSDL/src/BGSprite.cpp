#include "GraphicsEngineWithSDL.hpp"
#include "Actor/Actor.hpp"
#include "BGSprite.hpp"


using namespace Game;
using namespace WithSDL::Component;
using namespace WithSDL::Skin::Flyweight;

void BGSprite::setBGTextures(const std::vector<SDL_Texture*> &textures) noexcept {
    int count = 0;
    for (auto tex : textures) {
        BGTextures.emplace_back(BGTexture{
            tex, count * screenSize.x});
        ++count;
    }
}

void BGSprite::tryDraw() noexcept{
    for (auto &bg : BGTextures){
        // transform position: top-left to center
        Vector2D c_pos=Vector2D{bg.offset_x, 0.0} + screenSize/2.0;
        Angle::Radian rotation=0.0;

        SpriteForSDL sprite={
            SpinRect{
                Rect{c_pos, screenSize},
                rotation
            },
            bg.texture
        };

        if(not canvas.expired()){
            auto cv=std::dynamic_pointer_cast<
                WithSDL::SubEngine::GraphicsEngine
            >(canvas.lock());

            cv->draw(sprite);
        }
    }
}

void BGSprite::update(const Time& deltaTime) noexcept {
    for (auto &bg : BGTextures) {
        // 텍스처 위치(offset)를 스크롤 스피드만큼 왼쪽으로 이동.
        bg.offset_x += scrollSpeed * deltaTime;

        // 화면이 왼쪽으로 이동
        if (scrollSpeed < 0) {
            // 화면 왼쪽 밖으로 완전히 사라지면
            if (bg.offset_x < -screenSize.x) {
                // 오른쪽 끝으로 이동.
                bg.offset_x += BGTextures.size() * screenSize.x;
            }
        }
        // 화면이 오른쪽으로 이동
        else if (scrollSpeed > 0) {
            // 화면 오른쪽 밖으로 완전히 사라지면
            if (bg.offset_x > screenSize.x) {
                // 왼쪽 끝으로 이동.
                bg.offset_x -= BGTextures.size() * screenSize.x;
            }
        }
    }
}
