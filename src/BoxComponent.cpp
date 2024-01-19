#include <cassert>

#include "Component/BoxComponent.hpp"
#include "Component/MoveComponent.hpp"
#include "SubEngine/GraphicsEngine.hpp"

void BoxComponent::setTexture(const TrueColor &c, const Vector2 &size) noexcept {
    color=c;
    boxSize=size;
}

void BoxComponent::draw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    ColorRect rect = {
        {mc->position(), boxSize},
        mc->rotation(),
        color
    };

    Observable<ColorRect>::notify(rect);
}

void BoxComponent::injectDependency() noexcept{
    DrawComponent::injectDependency();

    assert(!graphicsEngine.expired());
    auto self=std::static_pointer_cast<BoxComponent>(shared_from_this());

    graphicsEngine.lock()->Observable<bool>::subscribe(self);
    Observable<ColorRect>::subscribe(graphicsEngine);
}
