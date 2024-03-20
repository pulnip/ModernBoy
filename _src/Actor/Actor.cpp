#include <algorithm>
#include "Actor/Actor.hpp"
#include "Component/Component.hpp"
#include "Component/Traits.hpp"

void Actor::update(const Game::Time& deltaTime) noexcept{
    if(state != State::Active) return;

    for(auto& c: ordered){
        c->update(deltaTime, *this);
    }
    updateActor(deltaTime);
}

void Actor::add(pComp ptr) noexcept{
    components.emplace(ptr->getType(), ptr);
    ordered.emplace(ptr);
}

using Type=ComponentTraits::Type;

std::optional<Actor::pComp> Actor::get(Type type) noexcept{
    auto it=components.find(type);
    if(it==components.end()){
        return std::nullopt;
    }
    return it->second;
}

void Actor::remove(Type type) noexcept{
    components.erase(type);
    std::erase_if(ordered, [type](const pComp& ptr){
        return ptr->getType()==type;
    });
}

bool Actor::UpdateOrder::operator()(
    const pComp& lhs, const pComp& rhs
) const noexcept{
    return lhs->getUpdateOrder() < rhs->getUpdateOrder();
}
