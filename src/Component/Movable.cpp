#include "Component/Movable.hpp"
#include "Engine/Logger.hpp"
#include "Skin.hpp"

using namespace Component;

Movable::Movable(
    std::weak_ptr<Actor::Vanilla> actor, int  updateOrder
) noexcept:
    Ability(actor, updateOrder),
    logger(std::make_unique<Engine::BindedLogger>("Movable", id))
{
    logger->debug("constructed");
}

Movable::~Movable(){
    logger->debug("destructed");
}

void Movable::update(const Game::Time& deltaTime) noexcept{
    attr.update(deltaTime);
}