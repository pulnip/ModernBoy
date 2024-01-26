#include "Skin.hpp"
#include "GameEngine/GameEngine.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "Actor/IActor.hpp"
#include "Component/MoveComponent.hpp"

template<typename T>
bool is_same(std::weak_ptr<T> lhs, std::weak_ptr<T> rhs){
    return (not lhs.owner_before(rhs)) && (not rhs.owner_before(lhs));
}

void PhysicsSimulator::setCollision(
    std::weak_ptr<IActor> who, std::weak_ptr<IActor> to
) noexcept{
    assert(!who.expired());
    wpm who_mc=std::static_pointer_cast<MoveComponent>(
        who.lock()->find(ComponentName::MoveComponent)
    );
    assert(!to.expired());
    wpm to_mc=std::static_pointer_cast<MoveComponent>(
        to.lock()->find(ComponentName::MoveComponent)
    );

    auto it=collisionMap.lower_bound(who_mc);

    if( it == collisionMap.end() ||
        not is_same(who_mc, it->first)
    ){
        // key not exist
        collisionMap.emplace_hint(it,
            who_mc, std::list<wpm>{to_mc}
        );
    } else{
        // key already exists
        it->second.emplace_back(to_mc);
    }
}

void PhysicsSimulator::appendMovable(wpm movable) noexcept{
    collisionMap.try_emplace(movable, std::list<wpm>());
}

PhysicsSimulator::AABB_model::AABB_model(const wpm& mc) noexcept{
    assert(!mc.expired());
    auto _attr=mc.lock()->attr();

    position=_attr.position.linear;
    velocity=_attr.velocity.linear;
    size=_attr.volume.size();
}

std::pair<bool, std::pair<float, PhysicsSimulator::CollisionHint>>
PhysicsSimulator::AABB(
    const AABB_model& colliding, const AABB_model& collided,
    const float& deltaTime
) noexcept{
    auto [ctime, aboutX]=whenCollide(colliding, collided);

    return{(std::abs(ctime) <= deltaTime), {ctime, aboutX}};
}

std::pair<Math::Real, PhysicsSimulator::CollisionHint>
PhysicsSimulator::whenCollide(
    const AABB_model& colliding, const AABB_model& collided
) noexcept{
    // center에 대한 opponent의 상대 위치
    auto relativePosition = colliding.position - collided.position;
    // center에 대한 opponent의 상대 속도
    auto relativeVelocity = colliding.velocity - collided.velocity;
    // 충돌 판정 박스
    const auto checkBox = (collided.size + colliding.size) / 2;
    assert(checkBox.x >= 0);
    assert(checkBox.y >= 0);

    // 1사분면으로 표준화
    if(relativePosition.x < 0){
        relativePosition.x = -relativePosition.x;
        relativeVelocity.x = -relativeVelocity.x;
    }
    if(relativePosition.y < 0){
        relativePosition.y = -relativePosition.y;
        relativeVelocity.y = -relativeVelocity.y;
    }
    // 여기서부터, pos == abs(pos)
    // => pos.x > 0 && pos.y > 0 에 대해서만 생각하면 됨.
    assert(relativePosition.x >= 0);
    assert(relativePosition.y >= 0);

    // 진행방향에 대해, 처음으로 경계를 만나는 시간
    auto xtime_min=Math::infinity;
    // xtime 외에 다른 해
    auto xtime_max=Math::infinity;

    // 속도가 충분히 작다면
    if(Math::NearZero(relativeVelocity.x)){
        // 충돌범위 안에 점이 있을 때
        if(relativePosition.x < checkBox.x){
            // 무한히 시간을 뒤로 돌려야 진행 반대방향에 있는 경계를 만남.
            xtime_min = -Math::infinity;
        }
        // 충돌 범위 밖에 점이 있을 때
        // 무한히 시간을 진행해도 어느 경계도 만나지 않음.
    }
    // v < 0일 때

    // (충돌 범위 바깥에 점이 있을 때)
    // pos - boxPos만큼 이동하면 첫 번째 충돌(순방향) --- (1번 충돌)
    // => (pos - boxPos) / (-v) = t1 (*0 < t1)
    // pos - (-boxPos)만큼 이동하면 다른 충돌 ---------- (2번 충돌))
    // => (pos - (-boxPos)) / (-v) = t2 (*0 < t1 < t2)

    // (충돌 박스 안에 점이 있다고 할 때)
    // boxPos - pos만큼 이동하면 다른 충돌 --------------(1번 충돌)
    // => (boxPos - pos) / v = t1 (*t1 < 0)
    // pos - (-boxPos)만큼 이동하면 첫 번째 충돌 --------(2번 충돌)
    // => (pos - (-boxPos)) / (-v) = t2 (*t1 < 0 < t2)

    // v > 0일 때 이하 동일
    // 다만 t2 < t1
    else{
        // (1번 충돌)
        xtime_min = (checkBox.x - relativePosition.x) /  relativeVelocity.x;
        // (2번 충돌)
        xtime_max = (checkBox.x + relativePosition.x) / -relativeVelocity.x;

        if(relativeVelocity.x > 0){
            std::swap(xtime_min, xtime_max);
        }
    }
    assert(xtime_min <= xtime_max);

    // y값에 대해서도 동일.
    auto ytime_min=Math::infinity;
    auto ytime_max=Math::infinity;

    if(Math::NearZero(relativeVelocity.y)){
        if(relativePosition.y < checkBox.y){
            ytime_min = -Math::infinity;
        }
    } else{
        ytime_min = (checkBox.y - relativePosition.y) /  relativeVelocity.y;
        ytime_max = (checkBox.y + relativePosition.y) / -relativeVelocity.y;

        if(relativeVelocity.y > 0){
            std::swap(ytime_min, ytime_max);
        }
    }
    assert(ytime_min <= ytime_max);

    // xtime_min < t < xtime_max와
    // ytime_min < t < ytime_max가
    // 겹치는 구간 ctime_min < t ctime_max를 찾음.
    const auto ctime_min=std::max(xtime_min, ytime_min);
    const auto ctime_max=std::min(xtime_max, ytime_max);

    // 겹치는 곳이 없다면,
    if(ctime_min > ctime_max){
        // 충돌 불가.
        return {Math::infinity, {false, false}};
    }
    // 겹친다면, 충돌 '가능'
    // 더 가까운 충돌 가능 시간을 반환.
    
    // 0 < ctime_min < ctime_max 이면,
    else if(0 < ctime_min){
        // ctime_min이 충돌 시간.
        return {
            ctime_min, {
                ctime_min == xtime_min, // 이 값이 x에 대해서 추론.
                ctime_min == ytime_min
            }
        };
    }
    // ctime_min < ctime_max < 0 이면,
    else if(ctime_max < 0){
        // 이하 동문.
        return {ctime_max, {ctime_max==xtime_max, ctime_max==ytime_max}};
    }
    // ctime_min < 0 < ctime_max 이면,
    else{
        // 지금이 충돌 가능 시간
        return {0.0, {true, true}};
    }
}

bool PhysicsSimulator::isGetCloser(
    const AABB_model& center, const AABB_model& opponent
) noexcept{
    // center에 대한 opponent의 상대 위치
    const auto relativePosition = opponent.position - center.position;
    // center에 대한 opponent의 상대 속도
    const auto relativeVelocity = opponent.velocity - center.velocity;

    // opponent가 center에 가까워지는 중.
    // = opponent의 속도가, 두 점 사이의 거리를 좁히는 쪽의 방향임.
    return relativePosition*relativeVelocity < 0;
}

void PhysicsSimulator::redoWithCollisionAndFlipRelativeVelocity(
    const float& totalTime, const float& collideTime,
    Attribute_2D& target, const Vector2& opponent,
    const CollisionHint& hint
) noexcept{
    // undo
    target.undo_update(totalTime);

    // redo until collide
    target.update(collideTime);

    // flip velocity
    auto& v=target.velocity.linear;
    if(hint.first){
#ifdef TOTALLY_INELASTIC_COLLISION
        v.x = opponent.x;
#else
        v.x = Math::reflect(v.x, opponent.x);
#endif
    }

    if(hint.second){
#ifdef TOTALLY_INELASTIC_COLLISION
        v.y = opponent.y
#else
        v.y = Math::reflect(v.y, opponent.y);
#endif
    }

    // redo after collide
    target.update(totalTime - collideTime);
}

void PhysicsSimulator::update(const float& deltaTime) noexcept{
    // TODO
    // remove if expired

    // check collision
    for(auto& [wpTarget, list]: collisionMap){
        const AABB_model target=wpTarget;

        bool isCollided=false;

        for(auto& wpOpponent: list){
            const AABB_model opponent=wpOpponent;

            // collision check by AABB
            auto [collide, timeWithHint]=AABB(target, opponent, deltaTime);

            if(collide){
                isCollided=true;

                auto [estimatedCollisionTime, hint]=timeWithHint;
                auto& c_attr=wpTarget.lock()->attr();
                redoWithCollisionAndFlipRelativeVelocity(
                    deltaTime, estimatedCollisionTime,
                    c_attr, opponent.velocity,
                    hint
                );
            }
        }

        // update move component
        if(not isCollided){
            wpTarget.lock()->attr().update(deltaTime);
        }
    }
}

void PhysicsSimulator::onNotify(Attribute_2D attr) noexcept{}
