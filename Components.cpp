#include <cassert>

#include <SDL2/SDL.h>

#include "Components.hpp"

#include "Actors.hpp"
#include "SubEngine.hpp"

// interface

Component::Component(const std::weak_ptr<Actor> owner) noexcept : owner(owner) {
    subscribe(owner);
}

void Component::postConstruct() noexcept {
    notify(PSMSG::Lifetime::CONSTRUCTED, shared_from_this());
}

// Concrete

// Collision Component

void CollisionComponent::update(const float &deltaTime) noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner = owner.lock();

    for (auto opponent : opponents) {
        assert(!opponent.expired() && "opponent(Actor): expired");
        const auto _opponent = opponent.lock();

        // 위치의 차이
        const auto pos_diff = _opponent->position - _owner->position;
        const auto pos_diff_abs = Vector2::abs(_opponent->position - _owner->position);
        // 충돌 판정 박스
        const auto col_box = (_owner->getSize() + _opponent->getSize()) / 2;

        // AABB 알고리즘으로 충돌 판정
        if (pos_diff_abs <= col_box) {
            // 충돌 후 처리
            const auto collision_result = col_box - pos_diff_abs;

            auto my_owmc = _owner->queryComponent(ComponentName::MoveComponent);
            auto op_owmc = _opponent->queryComponent(ComponentName::MoveComponent);
            assert(!(my_owmc.has_value() && op_owmc.has_value()));
            assert(my_owmc.value().expired() || op_owmc.value().expired());

            auto &myVel = std::dynamic_pointer_cast<MoveComponent>(
                              my_owmc.value().lock())
                              ->velocity;
            auto &opVel = std::dynamic_pointer_cast<MoveComponent>(
                              op_owmc.value().lock())
                              ->velocity;

            // 상대 속도
            const auto vel_diff = opVel - myVel;

            // x축 면이 닿았음.
            if (collision_result.x < collision_result.y) {
                // 해당 면으로 접근 중
                if (pos_diff.x * vel_diff.x < 0) {
#ifdef TOTALLY_INELASTIC_COLLISION
                    // 완전 비탄성 충돌
                    myVel.x = 0;
#else
                    myVel.x = Math::reflect(myVel.x, opVel.x);
#endif
                }
            } else {
                if (pos_diff.y * vel_diff.y < 0) {
#ifdef TOTALLY_INELASTIC_COLLISION
                    myVel.y = 0;
#else
                    myVel.y = Math::reflect(myVel.y, opVel.y);
#endif
                }
            }
        }
    }
}

// Draw Component

void DrawComponent::postConstruct() noexcept {
    Component::postConstruct();

#warning "subscribe graphics engine?"
}

// Box Component

void BoxComponent::draw() noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner = owner.lock();

    const auto pos = _owner->position;
    // size = base size * scale;
    const auto size = _owner->getSize();
    const auto rotation = _owner->rotation;

    ColorRect rect = {pos, size, rotation, color};

    Observable<ColorRect>::notify(rect);
}
void BoxComponent::setTexture(const TrueColor &color, const Vector2 &size) noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    this->color = color;
    owner.lock()->baseSize = size;
}

// Sprite Component

void SpriteComponent::draw() noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner = owner.lock();

    const auto pos = _owner->position;
    // size = base size * scale;
    const auto size = _owner->getSize();
    const auto rotation = _owner->rotation;

    SDL_Sprite sprite = {pos, size, rotation, texture};
    Observable<SDL_Sprite>::notify(sprite);
}
void SpriteComponent::setTexture(SDL_Texture *texture) noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner = owner.lock();

    SpriteComponent::texture = texture;

    int width, height;
    SDL_QueryTexture(texture,
                     nullptr,
                     nullptr,
                     &width,
                     &height);
    owner.lock()->baseSize = Vector2{static_cast<float>(width), static_cast<float>(height)};
}

// Animation Sprite Component

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

// Scrollable Background Sprite Component

void BGSpriteComponent::update(const float &deltaTime) noexcept {
    SpriteComponent::update(deltaTime);

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
            // transform position: top-left to center
            Vector2{bg.offset_x, 0.0f} + screenSize / 2,
            screenSize,
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

// Move Component

void MoveComponent::update(const float &deltaTime) noexcept {
    assert(!owner.expired() && "owner: expired");
    const auto _owner = owner.lock();

    _owner->position += velocity * deltaTime;
    _owner->rotation += rotationVelocity * deltaTime;
}

// Input Component

void InputComponent::update(const float &deltaTime) noexcept {
    uint8_t pressedCount = 0;

    for (auto &pair : keymap) {
        if (inputResult[pair.first]) {
            pressedCount += 1;
            pair.second();
        }
    }
    if (pressedCount == 0) {
        behaviorInStandBy();
    }
}

//