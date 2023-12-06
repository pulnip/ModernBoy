#include <algorithm>

#include "Actor.hpp"
#include "Components.hpp"
#include "Game.hpp"

void TransformComponent::update(float deltaTime){
    position += velocity * deltaTime;
}

static bool once=false;

void CollisionComponent::update(float deltaTime){
    for(auto opponent: opponents){
        // 위치의 차이
        auto pos_diff = tc->position - opponent->tc->position;
        // 충돌 판정 박스
        auto size_diff = sdc->size + opponent->sdc->size;

        // AABB 알고리즘으로 충돌 판정
        if(abs(pos_diff) <= size_diff){
            // 충돌 처리
            if(!once){
                SDL_Log("COLLIDE!\n");
                once=true;
            }
            // x축 반전 조건
            if(pos_diff.x * tc->velocity.x > 0){
                tc->velocity.x = -tc->velocity.x;
            }
            // y축 반전 조건
            if(pos_diff.y * tc->velocity.y > 0){
                tc->velocity.y = -tc->velocity.y;
            }
        }
        else{
            once=false;
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
