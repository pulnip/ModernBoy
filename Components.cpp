#include <algorithm>
#include <cassert>
#include <cmath>

#include <SDL2/SDL.h>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

// Component interface

Component::Component(
    const std::weak_ptr<Actor> owner,
    const int updateOrder)
noexcept :owner(owner), updateOrder(updateOrder) {
    assert(!owner.expired() && "Component argument owner: expired");
    assert(100<=updateOrder && updateOrder<400 &&
        "updateOrder: value not valid"
    );
    owner.lock()->appendComponent(this);
}
Component::~Component(){
    assert(!owner.expired() && "owner(Actor): expired");
    owner.lock()->removeComponent(this);
}

// Real Components

void CollisionComponent::update(const float deltaTime) noexcept{
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner=owner.lock();

    for(auto opponent: opponents){
        assert(!opponent.expired() && "opponent(Actor): expired");
        const auto _opponent=opponent.lock();

        // 위치의 차이
        const auto pos_diff = _opponent->getPosition() - _owner->getPosition();
        const auto pos_diff_abs = Vector2::abs(_opponent->getPosition() - _owner->getPosition());
        // 충돌 판정 박스
        const auto size_diff = (
            _owner->getScale()*_owner->getSize() +
            _opponent->getScale()*_opponent->getSize()
        )/2;

        // AABB 알고리즘으로 충돌 판정
        if(pos_diff_abs <= size_diff){            
            // 충돌 후 처리

            // 상대 속도
            const auto myVel = _owner->getVelocity();
            const auto opVel = _opponent->getVelocity();
            const auto vel_diff = opVel - myVel;

            const auto collision_result=size_diff - pos_diff_abs;
            // x축 면이 닿았음.
            if(collision_result.x < collision_result.y){
                // 해당 면으로 접근 중
                if(pos_diff.x * vel_diff.x < 0){
                    #ifdef TOTALLY_INELASTIC_COLLISION
                    // 완전 비탄성 충돌
                    _owner->setVelocity(
                        Vector2{0, _owner->getVelocity().y}
                    );
                    #else
                    _owner->setVelocity( Vector2{
                        Math::reflect(myVel.x, opVel.x),
                        myVel.y
                    });
                    #endif
                }
            }
            else{
                if(pos_diff.y * vel_diff.y < 0){
                    #ifdef TOTALLY_INELASTIC_COLLISION
                    _owner->setVelocity(
                        Vector2{_owner->getVelocity().x, 0}
                    );
                    #else
                    _owner->setVelocity( Vector2{
                        _owner->getVelocity().x,
                        Math::reflect(myVel.y, opVel.y)
                    });
                    #endif
                }
            }
        }
    }
}

void CollisionComponent::allow(const std::weak_ptr<Actor> opponent) noexcept{
    opponents.emplace_back(opponent);
}

void CollisionComponent::disallow(const std::weak_ptr<Actor> opponent) noexcept{
    opponents.erase(
        std::find(opponents.cbegin(), opponents.cend(), opponent)
    );
}

DrawComponent::DrawComponent(
    const std::weak_ptr<Actor> owner,
    const int drawOrder)
noexcept :Component(owner, 300), drawOrder(drawOrder){
    assert(!owner.expired() && "owner(Actor): expired");
    owner.lock()->getGame()->appendDrawable(this);
}
DrawComponent::~DrawComponent(){
    assert(!owner.expired() && "owner(Actor): expired");
    owner.lock()->getGame()->removeDrawable(this);
}

void BoxComponent::draw(const std::weak_ptr<SDL_Renderer> renderer) noexcept{
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner=owner.lock();

    assert(!renderer.expired() && "renderer: expired");
    const auto _renderer=renderer.lock().get();

    SDL_Rect rect;
    // 소유자의 배율 값으로 크기 조절
    rect.w = static_cast<int>(size.x * _owner->getScale());
    rect.h = static_cast<int>(size.y * _owner->getScale());
    // 중심 위치로 좌상단 좌표 계산
    rect.x = static_cast<int>(_owner->getPosition().x - rect.w/2);
    rect.y = static_cast<int>(_owner->getPosition().y - rect.h/2);


    SDL_SetRenderDrawColor(_renderer,
        color.r, color.g, color.b,
        color.a
    );
    SDL_RenderFillRect(_renderer, &rect);
}

void SpriteComponent::draw(const std::weak_ptr<SDL_Renderer> renderer) noexcept{
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner=owner.lock();

    assert(!renderer.expired() && "renderer: expired");
    const auto _renderer=renderer.lock().get();

    SDL_Rect rect;
    // 소유자의 배율 값으로 크기 조절
    rect.w = static_cast<int>(size.x * _owner->getScale());
    rect.h = static_cast<int>(size.y * _owner->getScale());
    // 중심 위치로 좌상단 좌표 계산
    rect.x = static_cast<int>(_owner->getPosition().x - rect.w/2);
    rect.y = static_cast<int>(_owner->getPosition().y - rect.h/2);

    assert(!texture.expired() && "texture: expired");
    SDL_RenderCopyEx(_renderer,
        texture.lock().get(),
        // 텍스처의 특정 영역 (nullptr은 전체영역)
        nullptr,
        // 어느 위치에, 어느 크기로 렌더링할 지 
        &rect,
        // 라디안을 각도로 변환
        Math::toDegree(_owner->getRotation()),
        // 회전 중심점
        nullptr,
        SDL_FLIP_NONE
    );
}

void SpriteComponent::setTexture(const std::weak_ptr<SDL_Texture> texture) noexcept{
    assert(!texture.expired() && "texture: expired");
    SpriteComponent::texture = texture;

    int width, height;
    SDL_QueryTexture(texture.lock().get(),
        nullptr, nullptr,
        &width, &height
    );
    size.x=width;
    size.y=height;
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
        bg.mOffset_x += scrollSpeed * deltaTime;

        // 화면이 왼쪽으로 이동
        if(scrollSpeed < 0){
            // 화면 왼쪽 밖으로 완전히 사라지면
            if(bg.mOffset_x < -screenSize.x){
                // 오른쪽 끝으로 이동.
                bg.mOffset_x += BGTextures.size() * screenSize.x;
            }
        }
        // 화면이 오른쪽으로 이동
        else if(scrollSpeed > 0){
            // 화면 오른쪽 밖으로 완전히 사라지면
            if(bg.mOffset_x > screenSize.x){
                // 왼쪽 끝으로 이동.
                bg.mOffset_x -= BGTextures.size() * screenSize.x;
            }
        }
    }
}

void BGSpriteComponent::draw(const std::weak_ptr<SDL_Renderer> renderer) noexcept{
	assert(!renderer.expired() && "renderer: expired");
    const auto _renderer=renderer.lock().get();
    
    // const auto origin = _owner->getPosition() - mScreenSize/2;
    for (auto& bg : BGTextures){
		SDL_Rect rect;
		rect.w = static_cast<int>(screenSize.x);
		rect.h = static_cast<int>(screenSize.y);
		// rect.x = static_cast<int>(_owner->getPosition().x - rect.w/2 + bg.mOffset.x);
		// rect.y = static_cast<int>(_owner->getPosition().y - rect.h/2 + bg.mOffset.y);
		rect.x = static_cast<int>(bg.mOffset_x);
		rect.y = 0.0f;

        assert(!bg.texture.expired() && "bg texture: expired");
        auto _texture=bg.texture.lock().get();
		SDL_RenderCopy(_renderer,
			_texture,
			nullptr, &rect
		);
	}
}

void BGSpriteComponent::setBGTextures(const std::vector<const std::weak_ptr<SDL_Texture>>& textures) noexcept{
    int count=0;
    for(auto tex: textures){
        assert(!tex.expired() && "tex: expired");

        BGTextures.emplace_back(BGTexture{
            tex, count*screenSize.x
        });
        ++count;
    }
}

void MoveComponent::update(const float deltaTime) noexcept{
    assert(!owner.expired() && "owner: expired");
    const auto _owner=owner.lock();

    if(!Math::NearZero(mAngularSpeed)){
        _owner->setRotation(
            _owner->getRotation() + mAngularSpeed*deltaTime
        );
    }
    if(!Math::NearZero(mForwardSpeed)){
        const auto pos = _owner->getPosition();
        _owner->setPosition(
            pos + Vector2::forward(_owner->getRotation())*mForwardSpeed*deltaTime
        );
    }
}

void InputComponent::processInput(const uint8_t* keyState) noexcept{
    short dir=0;
    if(keyState[mForwardKey]) dir += 1;
    if(keyState[mBackwardKey]) dir -= 1;
    setForwardSpeed(dir * mForwardMoveSpeed);

    dir=0;
    if(keyState[mClockwiseKey]) dir += 1;
    if(keyState[mCounterClockwiseKey]) dir -= 1;
    setAngularSpeed(dir * mAngularMoveSpeed);
}