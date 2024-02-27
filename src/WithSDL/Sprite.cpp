#include "Skin.hpp"
#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"
#include "WithSDL/Graphics.hpp"
#include "Component/Movable.hpp"
#include "WithSDL/Sprite.hpp"

using namespace WithSDL;

Sprite::Sprite(
    std::weak_ptr<Actor::Vanilla> actor,
    int drawOrder
) noexcept: Drawable(actor, drawOrder){}

void Sprite::draw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    Skin::Flyweight::Sprite sprite{
        mc->get().spinRect(),
        texture
    };
        
    std::static_pointer_cast<WithSDL::Graphics>(
        Graphics::get()
    )->draw(sprite);
}