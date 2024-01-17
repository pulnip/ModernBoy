#include "BGSpriteComponent.hpp"

void BGSpriteComponent::update(const float &deltaTime) noexcept {
    #warning "To Do"
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

void BGSpriteComponent::draw() noexcept {
    for (auto &bg : BGTextures) {
        SDL_Sprite sprite = {
            {// transform position: top-left to center
             Vector2{bg.offset_x, 0.0f} + screenSize / 2,
             screenSize},
            0.0,
            bg.texture};

        Observable<SDL_Sprite>::notify(sprite);
    }
}

void BGSpriteComponent::setBGTextures(const std::vector<SDL_Texture *> &textures) noexcept {
    int count = 0;
    for (auto tex : textures) {
        BGTextures.emplace_back(BGTexture{
            tex, count * screenSize.x});
        ++count;
    }
}
