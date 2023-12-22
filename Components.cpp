#include <algorithm>
#include <cassert>
#include <cmath>
#include <typeinfo>

#include <SDL2/SDL.h>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

// Real Components

void CollisionComponent::update(const float deltaTime) noexcept{
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner=owner.lock();

    for(auto opponent: opponents){
        assert(!opponent.expired() && "opponent(Actor): expired");
        const auto _opponent=opponent.lock();

        // 위치의 차이
        const auto pos_diff = _opponent->position - _owner->position;
        const auto pos_diff_abs = Vector2::abs(_opponent->position - _owner->position);
        // 충돌 판정 박스
        const auto col_box = (_owner->getSize() + _opponent->getSize())/2;

        // AABB 알고리즘으로 충돌 판정
        if(pos_diff_abs <= col_box){            
            // 충돌 후 처리
            const auto collision_result=col_box - pos_diff_abs;

            assert(!_owner->queryComponent(typeid(MoveComponent).name()).expired());
            assert(!_opponent->queryComponent(typeid(MoveComponent).name()).expired());
            auto& myVel=std::dynamic_pointer_cast<MoveComponent>(
                _owner->queryComponent(typeid(MoveComponent).name()).lock()
            )->velocity();
            auto& opVel=std::dynamic_pointer_cast<MoveComponent>(
                _opponent->queryComponent(typeid(MoveComponent).name()).lock()
            )->velocity();

            // 상대 속도
            const auto vel_diff = opVel - myVel;

            // x축 면이 닿았음.
            if(collision_result.x < collision_result.y){
                // 해당 면으로 접근 중
                if(pos_diff.x * vel_diff.x < 0){
                    #ifdef TOTALLY_INELASTIC_COLLISION
                    // 완전 비탄성 충돌
                    myVel.x=0;
                    #else
                    myVel.x=Math::reflect(myVel.x, opVel.x);
                    #endif
                }
            }
            else{
                if(pos_diff.y * vel_diff.y < 0){
                    #ifdef TOTALLY_INELASTIC_COLLISION
                    myVel.y=0;
                    #else
                    myVel.y=Math::reflect(myVel.y, opVel.y);
                    #endif
                }
            }
        }
    }
}

DrawComponent::DrawComponent(const std::weak_ptr<Actor> owner) noexcept: Component(owner){}

void DrawComponent::load(const std::weak_ptr<Component> self) noexcept{
    assert(!owner.lock()->getGame().expired() && "game: expired");
    owner.lock()->getGame().lock()->appendDrawable(
        std::static_pointer_cast<DrawComponent>(self.lock())
    );
}

void BoxComponent::draw(SDL_Renderer* renderer) noexcept{
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner=owner.lock();

    SDL_Rect rect;
    const auto size=_owner->getSize();
    const auto pos=_owner->position;
    // 소유자의 배율 값으로 크기 조절
    rect.w = static_cast<int>(size.x);
    rect.h = static_cast<int>(size.y);
    // 중심 위치로 좌상단 좌표 계산
    rect.x = static_cast<int>(pos.x - rect.w/2);
    rect.y = static_cast<int>(pos.y - rect.h/2);

    SDL_SetRenderDrawColor(renderer,
        color.r, color.g, color.b,
        color.a
    );
    SDL_RenderFillRect(renderer, &rect);
}

void BoxComponent::setTexture(const BoxComponent::Color& color, const Vector2& size) noexcept{
    assert(!owner.expired() && "owner(Actor): expired");
    BoxComponent::color=color;
    owner.lock()->baseSize=size;
}

void SpriteComponent::draw(SDL_Renderer* renderer) noexcept{
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner=owner.lock();

    SDL_Rect rect;
    const auto size=_owner->getSize();
    const auto pos=_owner->position;
    // 소유자의 배율 값으로 크기 조절
    rect.w = static_cast<int>(size.x);
    rect.h = static_cast<int>(size.y);
    // 중심 위치로 좌상단 좌표 계산
    rect.x = static_cast<int>(pos.x - rect.w/2);
    rect.y = static_cast<int>(pos.y - rect.h/2);

    SDL_RenderCopyEx(renderer,
        texture,
        // 텍스처의 특정 영역 (nullptr은 전체영역)
        nullptr,
        // 어느 위치에, 어느 크기로 렌더링할 지 
        &rect,
        // 라디안을 각도로 변환
        Math::toDegree(_owner->rotation),
        // 회전 중심점
        nullptr,
        SDL_FLIP_NONE
    );
}

void SpriteComponent::setTexture(SDL_Texture* texture) noexcept{
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner=owner.lock();
    SpriteComponent::texture = texture;

    int width, height;
    SDL_QueryTexture(texture,
        nullptr, nullptr,
        &width, &height
    );
    owner.lock()->baseSize=Vector2{static_cast<float>(width), static_cast<float>(height)};
}

void AnimSpriteComponent::update(const float deltaTime) noexcept{
    SpriteComponent::update(deltaTime);

    // 애니메이션에 사용된 텍스처 개수
    const int tex_num=animTextures.size();
    // 텍스처가 없을 경우
    if(tex_num <= 0) return;

    // 현재 프레임 갱신
    currFrame += animFPS * deltaTime;
    // 0 <= mCurrFrame < tex_num 이도록
    currFrame -= static_cast<int>(currFrame) / tex_num * tex_num;

    setTexture(animTextures[static_cast<int>(currFrame)]);
}

void BGSpriteComponent::update(const float deltaTime) noexcept{
    SpriteComponent::update(deltaTime);

    for(auto& bg: BGTextures){
        // 텍스처 위치(offset)를 스크롤 스피드만큼 왼쪽으로 이동.
        bg.offset_x += scrollSpeed * deltaTime;

        // 화면이 왼쪽으로 이동
        if(scrollSpeed < 0){
            // 화면 왼쪽 밖으로 완전히 사라지면
            if(bg.offset_x < -screenSize.x){
                // 오른쪽 끝으로 이동.
                bg.offset_x += BGTextures.size() * screenSize.x;
            }
        }
        // 화면이 오른쪽으로 이동
        else if(scrollSpeed > 0){
            // 화면 오른쪽 밖으로 완전히 사라지면
            if(bg.offset_x > screenSize.x){
                // 왼쪽 끝으로 이동.
                bg.offset_x -= BGTextures.size() * screenSize.x;
            }
        }
    }
}

void BGSpriteComponent::draw(SDL_Renderer* renderer) noexcept{    
    // const auto origin = _owner->position - mScreenSize/2;
    for (auto& bg : BGTextures){
		SDL_Rect rect;
		rect.w = static_cast<int>(screenSize.x);
		rect.h = static_cast<int>(screenSize.y);
		// rect.x = static_cast<int>(_owner->position.x - rect.w/2 + bg.mOffset.x);
		// rect.y = static_cast<int>(_owner->position.y - rect.h/2 + bg.mOffset.y);
		rect.x = static_cast<int>(bg.offset_x);
		rect.y = 0.0f;

		SDL_RenderCopy(renderer,
			bg.texture,
			nullptr, &rect
		);
	}
}

void BGSpriteComponent::setBGTextures(const std::vector<SDL_Texture*>& textures) noexcept{
    int count=0;
    for(auto tex: textures){
        BGTextures.emplace_back(BGTexture{
            tex, count*screenSize.x
        });
        ++count;
    }
}

void MoveComponent::update(const float deltaTime) noexcept{
    assert(!owner.expired() && "owner: expired");
    const auto _owner=owner.lock();

    _owner->position += velocity() * deltaTime;
}
void InputComponent::processInput(const uint8_t* keyState) noexcept{
    short dir=0;
    if(keyState[xPositiveKey]) dir += 1;
    if(keyState[xNegativeKey]) dir -= 1;
    velocity().x *= dir;

    dir=0;
    if(keyState[yPositiveKey]) dir += 1;
    if(keyState[yNegativeKey]) dir -= 1;
    velocity().y *= dir;
}

void AngularMoveComponent::update(const float deltaTime) noexcept{
    assert(!owner.expired() && "owner: expired");
    const auto _owner=owner.lock();

    _owner->position += velocity(_owner->rotation, forwardSpeed) * deltaTime;
    _owner->rotation += angularSpeed * deltaTime;
}
void InputComponentP::processInput(const uint8_t* keyState) noexcept{
    short dir=0;
    if(keyState[forwardKey]) dir += 1;
    if(keyState[backwardKey]) dir -= 1;
    setForwardSpeed(dir * forwardSpeedPreset);

    dir=0;
    if(keyState[clockwiseKey]) dir += 1;
    if(keyState[counterClockwiseKey]) dir -= 1;
    setAngularSpeed(dir * angularSpeedPreset);
}