#pragma once

#include <vector>

#include "SpriteComponent.hpp"

class AnimSpriteComponent : public SpriteComponent {
  public:
    // 애니메이션을 프레임마다 갱신
    void update(const float &deltaTime) noexcept override final;
    virtual ComponentName getName() const noexcept override {
        return ComponentName::AnimSpriteComponent;
    }

    // 애니메이션에 사용되는 텍스처 설정
    void setAnimTextures(const std::vector<class SDL_Texture *> &textures) {
        animTextures = textures;
    }
    
    // 애니메이션 FPS 세팅
    void setAnimFPS(const float fps) noexcept { animFPS = fps; }

  protected:
    AnimSpriteComponent() noexcept{
        drawOrder = 202;
    }

    std::vector<class SDL_Texture *> animTextures;
    // 현재 프레임
    float currFrame = 0.0f;
    int animFPS = 8;
};
