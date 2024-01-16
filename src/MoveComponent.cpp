#include <cassert>

#include "Actor/Actor.hpp"
#include "Component/MoveComponent.hpp"

void MoveComponent::update(const float &deltaTime) noexcept {
    assert(!owner.expired() && "owner: expired");
    const auto _owner = owner.lock();

    _owner->setPosition(_owner->getPosition() + velocity * deltaTime);
    _owner->setRotation(_owner->getRotation() + rotationVelocity * deltaTime);
}
