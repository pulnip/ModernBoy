#include <cassert>

#include "SDL2/SDL_render.h"

#include "Actor/Actor.hpp"
#include "Component/SpriteComponent.hpp"
#include "Skin.hpp"

void SpriteComponent::draw() noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner = owner.lock();

    const auto pos = _owner->position;
    // size = base size * scale;
    const auto size = _owner->getSize();
    const auto rotation = _owner->rotation;

    SDL_Sprite sprite = {{pos, size}, rotation, texture};
    Observable<SDL_Sprite>::notify(sprite);
}

void SpriteComponent::setTexture(SDL_Texture *texture) noexcept {
    assert(!owner.expired() && "owner(Actor): expired");
    const auto _owner = owner.lock();

    SpriteComponent::texture = texture;

    int width, height;
    SDL_QueryTexture(texture,
                     nullptr,
                     nullptr,
                     &width,
                     &height);
    owner.lock()->baseSize = Vector2{static_cast<float>(width), static_cast<float>(height)};
}
