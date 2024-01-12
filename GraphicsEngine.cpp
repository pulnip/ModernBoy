#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include "DrawComponent.hpp"
#include "GraphicsEngine.hpp"

bool GraphicsEngine::DrawOrder::operator()(const Drawable &lhs, const Drawable &rhs) const {
    return lhs->getDrawOrder() < rhs->getDrawOrder();
}

void GraphicsEngine::update(const float &deltaTime) noexcept {
    prepareRendering();

    for (auto &drawable : drawables) {
        drawable->draw();
    }

    finalizeRendering();
}
