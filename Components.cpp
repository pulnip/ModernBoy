#include <algorithm>
#include <cmath>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

Component::Component(Actor* owner, int updateOrder)
:mOwner(owner), mUpdateOrder(updateOrder){
    owner->AddComponent(this);
}
Component::~Component(){
    mOwner->RemoveComponent(this);
}

// Real Components

// void TransformComponent::update(float deltaTime){
//     position += velocity * deltaTime;
// }

void CollisionComponent::Update(float deltaTime){
    for(auto opponent: opponents){
        // 위치의 차이
        const auto pos_diff = opponent->GetPosition() - mOwner->GetPosition();
        const auto pos_diff_abs = abs(opponent->GetPosition() - mOwner->GetPosition());
        // 충돌 판정 박스
        const auto size_diff = (
            mOwner->GetScale()*mOwner->GetSize() +
            opponent->GetScale()*opponent->GetSize()
        )/2;

        // AABB 알고리즘으로 충돌 판정
        if(pos_diff_abs <= size_diff){            
            // 충돌 후 처리

            // 상대 속도
            const auto myVel = mOwner->GetVelocity();
            const auto opVel = opponent->GetVelocity();
            const auto vel_diff = opVel - myVel;

            const auto collision_result=size_diff - pos_diff_abs;
            // x축 면이 닿았음.
            if(collision_result.x < collision_result.y){
                // 해당 면으로 접근 중
                if(pos_diff.x * vel_diff.x < 0){
                    #ifdef TOTALLY_INELASTIC_COLLISION
                    // 완전 비탄성 충돌
                    mOwner->SetVelocity(
                        Vector2{0, mOwner->GetVelocity().y}
                    );
                    #else
                    mOwner->SetVelocity( Vector2{
                        Math::reflect(myVel.x, opVel.x),
                        myVel.y
                    });
                    #endif
                }
            }
            else{
                if(pos_diff.y * vel_diff.y < 0){
                    #ifdef TOTALLY_INELASTIC_COLLISION
                    mOwner->SetVelocity(
                        Vector2{mOwner->GetVelocity().x, 0}
                    );
                    #else
                    mOwner->SetVelocity( Vector2{
                        mOwner->GetVelocity().x,
                        Math::reflect(myVel.y, opVel.y)
                    });
                    #endif
                }
            }
        }
    }
}

void CollisionComponent::Allow(const Actor* opponent){
    opponents.emplace_back(opponent);
}

void CollisionComponent::Disallow(const Actor* opponent){
    opponents.erase(
        std::find(opponents.cbegin(), opponents.cend(), opponent)
    );
}

void ControlComponent::Update(float deltaTime){
    auto keystate=mOwner->GetGame()->state;

    int paddleDir=0;
    if(keystate[SDL_SCANCODE_W]){
        paddleDir -= 1;
    }
    if(keystate[SDL_SCANCODE_S]){
        paddleDir += 1;
    }

    mOwner->SetVelocity(Vector2{
        mOwner->GetVelocity().x, paddleDir*mOwner->GetVelocity().y
    });
}

DrawComponent::DrawComponent(Actor* owner, int drawOrder)
:Component(owner), mDrawOrder(drawOrder){
    mOwner->GetGame()->AddDrawable(this);
}
DrawComponent::~DrawComponent(){
    mOwner->GetGame()->RemoveDrawable(this);
}

void BoxComponent::Draw(SDL_Renderer* renderer){
    if(!mColor) return;

    SDL_Rect rect;
    // 소유자의 배율 값으로 크기 조절
    rect.w = static_cast<int>(mSize.x * mOwner->GetScale());
    rect.h = static_cast<int>(mSize.y * mOwner->GetScale());
    // 중심 위치로 좌상단 좌표 계산
    rect.x = static_cast<int>(mOwner->GetPosition().x - rect.w/2);
    rect.y = static_cast<int>(mOwner->GetPosition().y - rect.h/2);

    SDL_SetRenderDrawColor(renderer,
        mColor->r, mColor->g, mColor->b,
        mColor->a
    );
    SDL_RenderFillRect(renderer, &rect);
}

void SpriteComponent::Draw(SDL_Renderer* renderer){
    if(!mTexture) return;

    SDL_Rect rect;
    // 소유자의 배율 값으로 크기 조절
    rect.w = static_cast<int>(mSize.x * mOwner->GetScale());
    rect.h = static_cast<int>(mSize.y * mOwner->GetScale());
    // 중심 위치로 좌상단 좌표 계산
    rect.x = static_cast<int>(mOwner->GetPosition().x - rect.w/2);
    rect.y = static_cast<int>(mOwner->GetPosition().y - rect.h/2);

    SDL_RenderCopyEx(renderer,
        mTexture,
        // 텍스처의 특정 영역 (nullptr은 전체영역)
        nullptr,
        // 어느 위치에, 어느 크기로 렌더링할 지 
        &rect,
        // 라디안을 각도로 변환
        Math::ToDegree(mOwner->GetRotation()),
        // 회전 중심점
        nullptr,
        SDL_FLIP_NONE
    );
}

void SpriteComponent::SetTexture(SDL_Texture* texture){
    mTexture = texture;

    int width, height;
    SDL_QueryTexture(texture,
        nullptr, nullptr,
        &width, &height
    );
    mSize.x=width;
    mSize.y=height;
}

void AnimSpriteComponent::Update(float deltaTime) noexcept{
    SpriteComponent::Update(deltaTime);

    // 애니메이션에 사용된 텍스처 개수
    const int tex_num=mAnimTextures.size();
    // 텍스처가 없을 경우
    if(tex_num <= 0) return;

    // 현재 프레임 갱신
    mCurrFrame += mAnimFPS * deltaTime;
    // 0 <= mCurrFrame < tex_num 이도록
    mCurrFrame -= static_cast<int>(mCurrFrame) / tex_num * tex_num;

    SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
}