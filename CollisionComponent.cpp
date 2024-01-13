#include <cassert>

#include "Actor.hpp"
#include "CollisionComponent.hpp"
#include "MoveComponent.hpp"

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
