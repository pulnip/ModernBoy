#include "Engine/Graphics.hpp"
#include "Engine/Logger.hpp"
#include "Component/Drawable.hpp"

using namespace Engine;

Graphics::Graphics() noexcept:
    logger(std::make_unique<BindedLogger>("Graphics", "Base"))
{
    logger->debug("constructed");
}

Graphics::~Graphics(){
    logger->debug("destructed");
}

void Graphics::update(const Game::Time& deltaTime) noexcept{
    clearScreen();

    logger->debug("remove expired drawable");
    std::erase_if(drawables, [](const auto& d){
        return d.expired();
    });

    logger->debug("update drawable");
    for(auto& d: drawables){
        if(not d.expired()){
            d.lock()->draw();
        }
    }

    swapBuffer();
}

void Graphics::append(wp ptr) noexcept{
    drawables.emplace(ptr);
}

bool Graphics::DrawOrder::operator()(
    const wp& lhs, const wp& rhs
) const{
    assert(not lhs.expired() and not rhs.expired());
    return lhs.lock()->getDrawOrder() < rhs.lock()->getDrawOrder();
}