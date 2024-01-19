#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include "Skin.hpp"
#include "Component/DrawComponent.hpp"
#include "SubEngine/GraphicsEngine.hpp"

bool GraphicsEngine::DrawOrder::operator()(const Drawable &lhs, const Drawable &rhs) const {
    return lhs->getDrawOrder() < rhs->getDrawOrder();
}

void GraphicsEngine::update(const float &deltaTime) noexcept {
    prepareRendering();
    
    Observable<bool>::notify(true);

    finalizeRendering();
}

void NullGraphicsEngine::onNotify(ColorRect rect) noexcept{}
