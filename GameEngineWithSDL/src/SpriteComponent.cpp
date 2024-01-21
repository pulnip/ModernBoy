#include <cassert>
#include <SDL2/SDL_render.h>

#include "GraphicsEngineWithSDL.hpp"
#include "Component/MoveComponent.hpp"
#include "SpriteComponent.hpp"

void SpriteComponent::draw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    SpriteForSDL sprite = {
        mc->attr.spinRect(),
        texture
    };

    Observable<SpriteForSDL>::notify(sprite);
}

void SpriteComponent::injectDependency() noexcept{
    DrawComponent::injectDependency();
    auto self=std::static_pointer_cast<SpriteComponent>(shared_from_this());

    assert(!graphicsEngine.expired());
    auto geSDL=std::dynamic_pointer_cast<GraphicsEngineWithSDL>(
        graphicsEngine.lock()
    );
    geSDL->Observable<bool>::subscribe(self);
    Observable<SpriteForSDL>::subscribe(geSDL);
}
