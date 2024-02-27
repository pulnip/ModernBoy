#include "Component/AI.hpp"

using namespace Component;

AI::AI(std::weak_ptr<Actor::Vanilla> actor
) noexcept: Ability(actor, 250){}
