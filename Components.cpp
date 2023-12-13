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

    // paddle

    // int paddleDir=0;
    // if(keystate[SDL_SCANCODE_W]){
    //     paddleDir -= 1;
    // }
    // if(keystate[SDL_SCANCODE_S]){
    //     paddleDir += 1;
    // }

    // mOwner->SetVelocity(Vector2{
    //     mOwner->GetVelocity().x, paddleDir*mOwner->GetVelocity().y
    // });

    Vector2 newVel;
    if(keystate[SDL_SCANCODE_W]){
        newVel.y += -300.0f;
    }
    if(keystate[SDL_SCANCODE_A]){
        newVel.x += -300.0f;
    }
    if(keystate[SDL_SCANCODE_S]){
        newVel.y += 300.0f;
    }
    if(keystate[SDL_SCANCODE_D]){
        newVel.x += 300.0f;
    }
    mOwner->SetVelocity(newVel);
}

DrawComponent::DrawComponent(Actor* owner, int drawOrder)
:Component(owner, 300), mDrawOrder(drawOrder){
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

void BGSpriteComponent::SetBGTextures(const std::vector<SDL_Texture*>& textures){
    int count=0;
    for(auto tex: textures){
        mBGTextures.emplace_back(BGTexture{
            tex, count*mScreenSize.x
        });
        ++count;
    }
}

void BGSpriteComponent::Update(float deltaTime) noexcept{
    SpriteComponent::Update(deltaTime);

    for(auto& bg: mBGTextures){
        // 텍스처 위치(offset)를 스크롤 스피드만큼 왼쪽으로 이동.
        bg.mOffset_x += mScrollSpeed * deltaTime;

        // 화면이 왼쪽으로 이동
        if(mScrollSpeed < 0){
            // 화면 왼쪽 밖으로 완전히 사라지면
            if(bg.mOffset_x < -mScreenSize.x){
                // 오른쪽 끝으로 이동.
                bg.mOffset_x += mBGTextures.size() * mScreenSize.x;
            }
        }
        // 화면이 오른쪽으로 이동
        else if(mScrollSpeed > 0){
            // 화면 오른쪽 밖으로 완전히 사라지면
            if(bg.mOffset_x > mScreenSize.x){
                // 왼쪽 끝으로 이동.
                bg.mOffset_x -= mBGTextures.size() * mScreenSize.x;
            }
        }
    }
}

void BGSpriteComponent::Draw(SDL_Renderer* renderer) noexcept{
	// const auto origin = mOwner->GetPosition() - mScreenSize/2;
    for (auto& bg : mBGTextures){
		SDL_Rect rect;
		rect.w = static_cast<int>(mScreenSize.x);
		rect.h = static_cast<int>(mScreenSize.y);
		// rect.x = static_cast<int>(mOwner->GetPosition().x - rect.w/2 + bg.mOffset.x);
		// rect.y = static_cast<int>(mOwner->GetPosition().y - rect.h/2 + bg.mOffset.y);
		rect.x = static_cast<int>(bg.mOffset_x);
		rect.y = 0.0f;

		SDL_RenderCopy(renderer,
			bg.mTexture,
			nullptr, &rect
		);
	}
}

void MoveComponent::Update(float deltaTime){
    if(!Math::NearZero(mAngularSpeed)){
        mOwner->SetRotation(
            mOwner->GetRotation() + mAngularSpeed*deltaTime
        );
    }
    if(!Math::NearZero(mForwardSpeed)){
        const auto pos = mOwner->GetPosition();
        mOwner->SetPosition(
            pos + normalize(pos)*mForwardSpeed*deltaTime
        );
    }
}