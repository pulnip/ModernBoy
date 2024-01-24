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

bool PhysicsSimulator::AABB(
    const AABB_model& lhs, const AABB_model& rhs
) noexcept{
    // 위치의 차이
    const auto pos_diff = rhs.position - lhs.position;
    const auto pos_diff_abs = Vector2::abs(pos_diff);
    // 충돌 판정 박스
    const auto col_box = (lhs.size + rhs.size) / 2;

    // lhs에 대한 rhs의 위치가, 둘의 충돌범위 안에 있을 때
    return pos_diff_abs <= col_box;
}

bool PhysicsSimulator::isGetCloser(
    const AABB_model& center, const AABB_model& opponent
) noexcept{
    // center에 대한 opponent의 상대 위치
    const auto r_pos = opponent.position - center.position;
    // center에 대한 opponent의 상대 속도
    const auto r_vel = opponent.velocity - center.velocity;

    return r_pos*r_vel > 0;
}

void PhysicsSimulator::update(const float& deltaTime) noexcept{
    // remove if expired

    for(auto& [wpCenter, list]: collisionMap){
        AABB_model center=wpCenter;
        for(auto& wpOpponent: list){
            AABB_model opponent=wpOpponent;

            // collision check by AABB
            if( AABB(center, opponent) && 
                isGetCloser(center, opponent)
            ){
                // TODO
                // flip center
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


