#include "Math.hpp"
#include "Actor/Actor.hpp"
#include "AnimSpriteComponent.hpp"

void AnimSpriteComponent::update(const float &deltaTime) noexcept {
    // 애니메이션에 사용된 텍스처 개수
    const Math::Real tex_num = animTextures.size();
    // 텍스처가 없을 경우
    if (tex_num < 1) return;

    // 현재 프레임 갱신
    currFrame += animFPS * deltaTime;
    // 0 <= mCurrFrame < tex_num 이도록
    currFrame = Math::wrap(
        static_cast<Math::Real>(currFrame),
        0.0, tex_num
    );

    setTexture(animTextures[static_cast<int>(currFrame)]);
}
