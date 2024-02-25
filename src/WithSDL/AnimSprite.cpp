#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"
#include "WithSDL/AnimSprite.hpp"

using namespace WithSDL;
using namespace My::Math;

AnimSprite::AnimSprite(
    std::weak_ptr<Actor::Vanilla> actor
) noexcept:
    Sprite(actor, 301),
    logger(std::make_unique<Engine::BindedLogger>("Sprite", "Anim"))
{
    logger->debug("constructed");
}

AnimSprite::~AnimSprite(){
    logger->debug("destructed");
}

void AnimSprite::update(const Game::Time& deltaTime) noexcept {
    // 애니메이션에 사용된 텍스처 개수
    const Number::Real tex_num = animTextures.size();
    // 텍스처가 없을 경우
    if (tex_num < 1) return;

    // 현재 프레임 갱신
    currFrame += animFPS * deltaTime;
    // 0 <= mCurrFrame < tex_num 이도록
    currFrame = wrap(currFrame, 0.0, tex_num);

    setTexture(animTextures[static_cast<int>(currFrame)]);
}