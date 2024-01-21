#pragma once

#include <vector>

#include "Math.hpp"
#include "SpriteComponent.hpp"

// 스크롤되는 배경
class BGSpriteComponent : public SpriteComponent {
  public:
    void update(const float &deltaTime) noexcept override;
    void draw() noexcept override;

    // 배경용 텍스처 설정
    void setBGTextures(const std::vector<class SDL_Texture *> &textures) noexcept;
    void setScreenSize(const Vector2 &size) noexcept { screenSize = size; }
    float getScrollSpeed() const noexcept { return scrollSpeed; }
    void setScrollSpeed(const float speed) noexcept { scrollSpeed = speed; }

  protected:
    BGSpriteComponent() noexcept{
        drawOrder = 100;
    }

  private:
    ComponentName getName() const noexcept override final{
        return ComponentName::BGSpriteComponent;
    }

  private:
    struct BGTexture {
        class SDL_Texture *texture;
        // 화면이 시작하는 위치
        float offset_x;
    };
    std::vector<BGTexture> BGTextures;
    Vector2 screenSize;
    float scrollSpeed = 0.0f;
};
