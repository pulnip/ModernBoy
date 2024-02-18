#include <cassert>

#include "Skin.hpp"
#include "Component/ColoredBody.hpp"
#include "Component/Movable.hpp"
#include "Actor/Actor.hpp"
#include "SubEngine/GraphicsEngine.hpp"

void ColoredBody::setTexture(const TrueColor &c) noexcept {
    color=c;
}

void ColoredBody::draw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    ColorRect rect = {
        mc->attr().rect(),
        color
    };

    UniqueObservable<ColorRect>::notify(rect);
}

void ColoredBody::setAttribute() noexcept{
    UniqueObservable<ColorRect>::subscribe(graphicsEngine);
}
