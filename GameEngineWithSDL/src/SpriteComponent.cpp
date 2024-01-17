#include <cassert>
#include <SDL2/SDL_render.h>

#include "GraphicsEngineWithSDL.hpp"
#include "Component/MoveComponent.hpp"
#include "SpriteComponent.hpp"

void SpriteComponent::draw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    SDL_Sprite sprite = {
        {mc->position(), spriteSize},
        mc->rotation(),
        texture
    };

    Observable<SDL_Sprite>::notify(sprite);
}

void SpriteComponent::setTexture(SDL_Texture *texture) noexcept {
    this->texture = texture;

    int width, height;
    SDL_QueryTexture(texture,
                     nullptr,
                     nullptr,
                     &width,
                     &height);
    spriteSize={static_cast<float>(width), static_cast<float>(height)};
}

void SpriteComponent::injectDependency() noexcept{
    DrawComponent::injectDependency();
    assert(!graphicsEngine.expired());
    Observable<SDL_Sprite>::subscribe(
        std::dynamic_pointer_cast<GraphicsEngineWithSDL>(graphicsEngine.lock())
    );
}
