#include <cassert>

#include "Skin.hpp"
#include "Component/BoxComponent.hpp"
#include "Component/MoveComponent.hpp"
#include "Actor/Actor.hpp"
#include "SubEngine/GraphicsEngine.hpp"

void BoxComponent::setTexture(const TrueColor &c) noexcept {
    color=c;
}

void BoxComponent::draw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    ColorRect rect = {
        mc->attr().rect(),
        color
    };

    UniqueObservable<ColorRect>::notify(rect);
}

void BoxComponent::setAttribute() noexcept{
    UniqueObservable<ColorRect>::subscribe(graphicsEngine);
}
