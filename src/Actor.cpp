#include <cassert>

#include "PubSubMessage.hpp"
#include "SubEngine/ActorManager.hpp"
#include "Actor/Actor.hpp"
#include "Component/MoveComponent.hpp"

void Actor::update(const float& deltaTime) noexcept {
    if (state!=State::EActive) return;

    for (auto& comp: orderedComponents) {
        comp->update(deltaTime);
    }
    updateActor(deltaTime);
}

std::shared_ptr<IComponent>
Actor::find(const ComponentName name) noexcept {
    auto result=components.find(name);
    if(result==components.end()){
        return nullptr;
    }
    return result->second;
}

std::optional<std::shared_ptr<ISubEngine>>
Actor::query(const SubEngineName name) noexcept{
    assert(!owner.expired());
    auto manager=owner.lock();

    return manager->query(name);
}

void Actor::onNotify(Lifetime msg, std::shared_ptr<Component> comp) noexcept{
    std::shared_ptr<IComponent> ic=comp;

    switch(msg){
    case Lifetime::CONSTRUCTED:
        components.emplace(ic->getName(), ic);
        orderedComponents.emplace(ic);
        break;
    case Lifetime::DESTRUCTED:
        components.erase(ic->getName());
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
