#include <cassert>

#include "Actor/Actor.hpp"
#include "Component/MoveComponent.hpp"

void MoveComponent::injectDependency() noexcept{
#warning "Not Impl"
}

void MoveComponent::update(const float &deltaTime) noexcept {
    assert(!owner.expired() && "owner: expired");
    const auto _owner = owner.lock();

    Math::Real dt=deltaTime;
    attr.update(dt);
}
