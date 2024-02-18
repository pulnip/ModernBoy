#include <cassert>

#include "Skin.hpp"
#include "Component/ColoredBody.hpp"
#include "Component/Movable.hpp"
#include "Actor/Actor.hpp"
#include "SubEngine/GraphicsEngine.hpp"

using namespace Game;
using namespace Game::Component;

void ColoredBody::setTexture(pColor c) noexcept {
    color=c;
}

void ColoredBody::tryDraw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    Skin::Flyweight::ColorRect rect = {
        mc->attr->rect(),
        *color
    };

    if(not canvas.expired()){
        auto cv=canvas.lock();
        cv->draw(rect);
    }
}
