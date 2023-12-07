#include <algorithm>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

Component::Component(Actor* owner, int updateOrder)
:mOwner(owner), mUpdateOrder(updateOrder){}
Component::~Component(){}

void Component::update(float deltaTime){}

// Real Components

void TransformComponent::update(float deltaTime){
    position += velocity * deltaTime;
}

void CollisionComponent::update(float deltaTime){
    for(auto opponent: opponents){
        // 위치의 차이
        auto pos_diff = opponent->tc->position - tc->position;
        auto pos_diff_abs = abs(opponent->tc->position - tc->position);
        // 충돌 판정 박스
        auto size_diff = (sdc->size + opponent->sdc->size)/2;

        // AABB 알고리즘으로 충돌 판정
        if(pos_diff_abs <= size_diff){
            // 충돌 후 처리

            // 상대 속도
            auto vel_diff = opponent->tc->velocity - tc->velocity;

            auto x_diff = size_diff.x - pos_diff_abs.x;
            auto y_diff = size_diff.y - pos_diff_abs.y;
            // x축 면이 닿았음.
            if(x_diff < y_diff){
                // 해당 면으로 접근 중
                if(pos_diff.x * vel_diff.x < 0){
                    #ifdef TOTALLY_INELASTIC_COLLISION
                    // 완전 비탄성 충돌
                    tc->velocity.x=0;
                    #else
                    tc->velocity.x = -tc->velocity.x;
                    #endif
                }
            }
            else{
                if(pos_diff.y * vel_diff.y < 0){
                    #ifdef TOTALLY_INELASTIC_COLLISION
                    tc->velocity.y=0;
                    #else
                    tc->velocity.y = -tc->velocity.y;
                    #endif
                }
            }
        }
    }
}

void CollisionComponent::collideAllow(const CollisionComponent* cc){
    opponents.emplace_back(cc);
}

void CollisionComponent::collideDisallow(const CollisionComponent* cc){
    opponents.erase(
        std::find(opponents.cbegin(), opponents.cend(), cc)
    );
}

void ControlComponent::update(float deltaTime){
    auto keystate=game->state;

    int paddleDir=0;
    if(keystate[SDL_SCANCODE_W]){
        paddleDir -= 1;
    }
    if(keystate[SDL_SCANCODE_S]){
        paddleDir += 1;
    }

    tc->velocity.y *= paddleDir;
}
