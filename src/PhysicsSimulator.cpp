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

PhysicsSimulator::AABB_model::AABB_model(const wpm& mc) noexcept{
    assert(!mc.expired());
    auto _attr=mc.lock()->attr();

    position=_attr.position.linear;
    velocity=_attr.velocity.linear;
    size=_attr.volume.size();
}

std::pair<bool, std::pair<float, bool>> PhysicsSimulator::AABB(
    const AABB_model& center, const AABB_model& opponent,
    const float& deltaTime
) noexcept{
    auto [ctime, aboutX]=whenCollide(center, opponent);

    return{(ctime <= deltaTime), {ctime, aboutX}};
}

std::pair<Math::Real, bool> PhysicsSimulator::whenCollide(
    const AABB_model& center, const AABB_model& opponent
) noexcept{
    // center에 대한 opponent의 상대 위치
    auto relativePosition = opponent.position - center.position;
    // center에 대한 opponent의 상대 속도
    auto relativeVelocity = opponent.velocity - center.velocity;
    // 충돌 판정 박스
    const auto checkBox = (center.size + opponent.size) / 2;

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

    // v가 0에 가까우면, 충돌하지 않음.(=충돌 시점이 무한대)
    auto xtime = Math::infinity;
    if(not Math::NearZero(relativeVelocity.x)){
        // x축 기준으로, opponent가 center에 만나는 시간.
        xtime = (checkBox.x - relativePosition.x) / relativeVelocity.x;
        
        // time < 0은 이미 만났다는 뜻이므로,
        xtime = std::max(0.0, xtime);
    }

    auto ytime = Math::infinity;
    if(not Math::NearZero(relativeVelocity.y)){
        // y축 기준으로, opponent가 center에 만나는 시간.
        ytime=(checkBox.y-relativePosition.y) / relativeVelocity.y;

        // time < 0은 이미 만났다는 뜻이므로,
        ytime = std::max(0.0, ytime);
    }

    // x와 y중 충돌시점이 더 가까운 걸 기준으로 판단.
    return{std::min(xtime, ytime), (xtime < ytime)};
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
    Attribute_2D& attr, const Vector2& opponent,
    bool flipX
) noexcept{
    // undo
    attr.undo_update(totalTime);

    // redo until collide
    attr.update(collideTime);

    // flip velocity
    auto& v=attr.velocity.linear;
    if(flipX){
#ifdef TOTALLY_INELASTIC_COLLISION
        v.x = opponent.x;
#else
        v.x = Math::reflect(v.x, opponent.x);
#endif
    } else{
#ifdef TOTALLY_INELASTIC_COLLISION
        v.y = opponent.y
#else
        v.y = Math::reflect(v.y, opponent.y);
#endif
    }

    // redo after collide
    attr.update(totalTime - collideTime);
}

void PhysicsSimulator::update(const float& deltaTime) noexcept{
    // TODO
    // remove if expired

    // update move component here

    for(auto& [wpCenter, list]: collisionMap){
        const AABB_model center=wpCenter;
        for(auto& wpOpponent: list){
            const AABB_model opponent=wpOpponent;

            // collision check by AABB
            auto [collide, pair]=AABB(center, opponent, deltaTime);
            if(collide){
                auto [ect, x]=pair;
                auto& c_attr=wpCenter.lock()->attr();

                redoWithCollisionAndFlipRelativeVelocity(
                    deltaTime, ect,
                    c_attr, opponent.velocity,
                    x
                );
            }
        }
    }
}

void PhysicsSimulator_default::update(const float &deltaTime) noexcept {
#warning "Not defined"
}

void PhysicsSimulator_default::onNotify(Attribute_2D attr) noexcept {
#warning "Not defined"
}


