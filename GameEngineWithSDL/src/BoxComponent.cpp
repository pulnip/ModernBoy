#include <cassert>

#include "Actor/Actor.hpp"
#include "Component/BoxComponent.hpp"

void BoxComponent::draw() noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner = owner.lock();

    const auto pos = _owner->position;
    // size = base size * scale;
    const auto size = _owner->getSize();
    const auto rotation = _owner->rotation;

    ColorRect rect = {{pos, size}, rotation, color};

    this->Observable<ColorRect>::notify(rect);
}

void BoxComponent::setTexture(const TrueColor &color, const Vector2 &size) noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    this->color = color;
    owner.lock()->baseSize = size;
}
