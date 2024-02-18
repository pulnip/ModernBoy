#include <cassert>

#include "SubEngine/ActorManager.hpp"
#include "Actor/Actor.hpp"
#include "Component/Movable.hpp"

void Actor::update(const float& deltaTime) noexcept {
    if (state!=State::EActive) return;

    for (auto& comp: orderedComponents) {
        comp->update(deltaTime);
    }
    updateActor(deltaTime);
}

std::shared_ptr<Component>
Actor::find(const Type name) noexcept {
    auto result=components.find(name);
    if(result==components.end()){
        return nullptr;
    }
    return result->second;
}

std::optional<std::shared_ptr<ISubEngine>>
Actor::query(const SubEngine::Type name) noexcept{
    assert(!owner.expired());
    auto manager=owner.lock();

    return manager->query(name);
}

void Actor::onNotify(Lifetime msg, std::shared_ptr<Component> comp) noexcept{
    std::shared_ptr<Component> ic=comp;

    switch(msg){
    case Lifetime::CONSTRUCTED:
        components.emplace(ic->getType(), ic);
        orderedComponents.emplace(ic);
        break;
    case Lifetime::DESTRUCTED:
        components.erase(ic->getType());
        orderedComponents.erase(ic);
        break;
    default:
        assert(false);
    }
}

bool Actor::UpdateOrder::operator()(
    const ptr& lhs, const ptr& rhs
) const noexcept{
    return lhs->getUpdateOrder() < rhs->getUpdateOrder();
}
