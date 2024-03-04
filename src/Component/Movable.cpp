#include "Component/Movable.hpp"
#include "Engine/Logger.hpp"
#include "Skin.hpp"

using namespace Component;

Movable::Movable(
    std::weak_ptr<Actor::Vanilla> actor
) noexcept: Ability(actor, 200){}

void Movable::update(const Game::Time& deltaTime) noexcept{
#warning "should move to physics simulator"
    // attr.update(deltaTime);
}