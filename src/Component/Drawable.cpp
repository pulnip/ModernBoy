#include <string>
#include "Skin.hpp"
#include "Component/Drawable.hpp"
#include "Engine/Logger.hpp"
#include "Actor/Vanilla.hpp"
#include "Component/Movable.hpp"

using namespace Component;

Drawable::Drawable(
    std::weak_ptr<Actor::Vanilla> actor,
    int drawOrder
) noexcept:
    Ability(actor, 300), drawOrder(drawOrder)
{
    if(not actor.expired()){
        auto query=actor.lock()->get(Type::Movable);
        if(query.has_value()){
            target=std::static_pointer_cast<Movable>(query.value());
        } else{
            logger.info("init Movable failed(Movable not found)");
        }
    } else{
        logger.info("init Drawable failed(actor not valid)");
    }

    // logger.debug(("DrawOrder is "+std::to_string(drawOrder)).c_str());
}