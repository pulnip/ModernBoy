#include <algorithm>
#include <cassert>

#include "Skin.hpp"
#include "GameEngine/GameEngine.hpp"
#include "Component/Drawable.hpp"
#include "SubEngine/GraphicsEngine.hpp"

using namespace Game;
using namespace Game::SubEngine;

bool GraphicsEngine::DrawOrder::operator()(
    const ptr& lhs, const ptr& rhs
) const{
    assert(not lhs.expired() and not rhs.expired());
    return lhs.lock()->getDrawOrder() < rhs.lock()->getDrawOrder();
}

void GraphicsEngine::update(const Time& deltaTime) noexcept{
    prepareRendering();

    std::erase_if(drawables, [](const auto& d){
        return d.expired();
    });

    for(auto& d: drawables){
        if(not d.expired()){
            d.lock()->tryDraw();
        }
    }

    finalizeRendering();
}

void GraphicsEngine::append(ptr d) noexcept{
    drawables.emplace(d);
}

void GraphicsEngine::postConstruct() noexcept{
    assert(not owner.expired());
    owner.lock()->setGraphicsEngine(shared_from_this());

}
