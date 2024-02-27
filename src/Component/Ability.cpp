#include <string>
#include "Component/Ability.hpp"

using namespace Component;
using namespace Engine;

int Ability::seed=0;

Ability::Ability(
    std::weak_ptr<Actor::Vanilla> actor, int updateOrder
) noexcept: id(++seed), actor(actor), updateOrder(updateOrder)
{
    if(actor.expired()){
        logger.info("actor not valid");
    }

    // logger.debug(("UpdateOrder is "+std::to_string(updateOrder)).c_str());
}