#include <cassert>

#include "Actor.hpp"
#include "MoveComponent.hpp"

void MoveComponent::update(const float &deltaTime) noexcept {
    assert(!owner.expired() && "owner: expired");
    const auto _owner = owner.lock();

    _owner->position += velocity * deltaTime;
    _owner->rotation += rotationVelocity * deltaTime;
}
