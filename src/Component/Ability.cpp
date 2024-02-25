#include "Component/Ability.hpp"

#include "Engine/Logger.hpp"

using namespace Component;

using namespace Engine;

int Ability::seed=0;

Ability::Ability(
    std::weak_ptr<Actor::Vanilla> actor, int updateOrder
) noexcept: id(++seed), actor(actor), updateOrder(updateOrder),
    logger(std::make_unique<Engine::BindedLogger>("Component", id))
{
    logger->debug("checking...");
    
    if(not actor.expired()){
        logger->debug("actor valid");
    } else{
        logger->info("actor not valid");
    }

    logger->debug("UpdateOrder is...");
    logger->debug(std::to_string(this->updateOrder).c_str());

    logger->debug("constructed");
}

Ability::~Ability(){
    logger->debug("destructed");
}