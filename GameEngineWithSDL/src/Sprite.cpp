#include <cassert>
#include <SDL2/SDL_render.h>

#include "GraphicsEngineWithSDL.hpp"
#include "Actor/Actor.hpp"
#include "Component/Movable.hpp"
#include "Sprite.hpp"

using namespace Game;
using namespace WithSDL::Component;

void Sprite::tryDraw() noexcept {
    assert(!target.expired());
    const auto mc=target.lock();

    SpriteForSDL sprite{
        mc->attr->spinRect(),
        texture
    };

    if(not canvas.expired()){
        auto ge=std::dynamic_pointer_cast<
            WithSDL::SubEngine::GraphicsEngine
        >(canvas.lock());
        
        ge->draw(sprite);
    }
}

void Sprite::setCanvas() noexcept{
    assert(not owner.expired());
    auto query=owner.lock()->query(Game::SubEngine::Type::GraphicsEngine);

    if(query.has_value()){
        canvas=std::dynamic_pointer_cast<
            WithSDL::SubEngine::GraphicsEngine
        >(query.value());
    }
}
