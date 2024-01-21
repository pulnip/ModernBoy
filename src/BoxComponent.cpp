#include <cassert>

#include "Component/BoxComponent.hpp"
#include "Component/MoveComponent.hpp"
#include "SubEngine/GraphicsEngine.hpp"

void BoxComponent::setTexture(const TrueColor &c) noexcept {
    color=c;
}

void BoxComponent::draw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    ColorRect rect = {
        mc->attr.rect(),
        color
    };

    Observable<ColorRect>::notify(rect);
}

void BoxComponent::injectDependency() noexcept{
    DrawComponent::injectDependency();
    auto self=std::static_pointer_cast<BoxComponent>(shared_from_this());

    assert(!graphicsEngine.expired());
    graphicsEngine.lock()->Observable<bool>::subscribe(self);
    Observable<ColorRect>::subscribe(graphicsEngine);
}
