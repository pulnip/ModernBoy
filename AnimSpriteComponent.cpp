#include "AnimSpriteComponent.hpp"

void AnimSpriteComponent::update(const float &deltaTime) noexcept {
    SpriteComponent::update(deltaTime);

    // 애니메이션에 사용된 텍스처 개수
    const int tex_num = animTextures.size();
    // 텍스처가 없을 경우
    if (tex_num <= 0)
        return;

    // 현재 프레임 갱신
    currFrame += animFPS * deltaTime;
    // 0 <= mCurrFrame < tex_num 이도록
    currFrame -= static_cast<int>(currFrame) / tex_num * tex_num;

    setTexture(animTextures[static_cast<int>(currFrame)]);
}
