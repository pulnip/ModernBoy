#include <cassert>

#include "Component/BoxComponent.hpp"
#include "Component/MoveComponent.hpp"
#include "SubEngine/GraphicsEngine.hpp"

void BoxComponent::draw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    ColorRect rect = {
        {mc->position(), boxSize},
        mc->rotation(),
        color
    };

    this->Observable<ColorRect>::notify(rect);
}

void BoxComponent::setTexture(const TrueColor &color, const Vector2 &size) noexcept {
    this->color=color;
    boxSize=size;
}

void BoxComponent::injectDependency() noexcept{
    DrawComponent::injectDependency();
    Observable<ColorRect>::subscribe(graphicsEngine);
}
