#include "Component/Drawable.hpp"

#include "Skin.hpp"
#include "Engine/Graphics.hpp"
#include "Actor/Vanilla.hpp"
#include "Component/Movable.hpp"

using namespace Component;

Colored::Colored(
    std::weak_ptr<Actor::Vanilla> actor,
    Skin::TrueColor color
) noexcept: Drawable(actor, 100), color(color){}

void Colored::draw() noexcept{
    assert(!target.expired());
    const auto mc=target.lock();

    Skin::Flyweight::ColorRect rect = {
        mc->get().rect(),
        color
    };

    Engine::Graphics::get()->draw(rect);
}
