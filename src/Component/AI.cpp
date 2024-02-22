#include "Component/AI.hpp"
#include "Engine/Logger.hpp"

using namespace Component;

AI::AI(
    std::weak_ptr<Actor::Vanilla> actor
) noexcept:
    Ability(actor, 250),
    logger(std::make_unique<Engine::BindedLogger>("Controllable", id))
{
    logger->debug("constructed");
}

AI::~AI(){
    logger->debug("destructed");
}