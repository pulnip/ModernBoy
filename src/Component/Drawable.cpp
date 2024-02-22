#include "Component/Drawable.hpp"

#include "Engine/Logger.hpp"
#include "Actor/Vanilla.hpp"

using namespace Component;

Drawable::Drawable(
    std::weak_ptr<Actor::Vanilla> actor, int updateOrder,
    int drawOrder
) noexcept:
    Ability(actor, updateOrder),
    drawOrder(drawOrder),
    logger(std::make_unique<Engine::BindedLogger>("Drawable", id))
{
    logger->debug("checking...");

    logger->debug("try to connect to Movable");
    if(not actor.expired()){
        logger->debug("try to get Movable");
        auto query=actor.lock()->get(Type::Movable);
        if(query.has_value()){
            target=std::static_pointer_cast<Movable>(query.value());
            logger->debug("init Movable succeed");
        } else{
            logger->info("init Movable failed(Movable not found)");
        }
    } else{
        logger->info("init Drawable failed(actor not valid)");
    }

    logger->debug("DrawOrder is...");
    log(this->drawOrder);

    logger->debug("constructed");
}
Drawable::~Drawable(){
    logger->debug("destructed");
}
