#include "Component/Drawable.hpp"

#include "Skin.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Graphics.hpp"
#include "Actor/Vanilla.hpp"
#include "Component/Movable.hpp"

using namespace Component;

Colored::Colored(
    std::weak_ptr<Actor::Vanilla> actor, int updateOrder,
    int drawOrder,
    Skin::TrueColor color
) noexcept:
    Drawable(actor, updateOrder, drawOrder), color(color),
    logger(std::make_unique<Engine::BindedLogger>("Colored", id))
{
    logger->debug("constructed");
}

Colored::~Colored(){
    logger->debug("destructed");
}

void Colored::draw() noexcept{
    logger->debug("try drawing...");
    assert(!target.expired());
    const auto mc=target.lock();

    Skin::Flyweight::ColorRect rect = {
        mc->get().rect(),
        color
    };

    Engine::Graphics::get()->draw(rect);
}
