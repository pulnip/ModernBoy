#include <cassert>

#include "PubSubMessage.hpp"
#include "SubEngine/ActorManager.hpp"
#include "Actor/Actor.hpp"
#include "Component/IComponent.hpp"

Actor::~Actor(){
    if(!owner.expired()){
        notify(Lifetime::DESTRUCTED);
    }
}

void Actor::postConstruct() noexcept{
    subscribe(owner);
    notify(Lifetime::CONSTRUCTED);
    
    injectDependency();
}

void Actor::onNotify(Lifetime msg, std::shared_ptr<IComponent> comp) noexcept {
    switch(msg){
    case Lifetime::CONSTRUCTED:
        components.emplace(comp->getName(), comp);
        orderedComponents.emplace(comp);
        break;
    case Lifetime::DESTRUCTED:
        components.erase(comp->getName());
        orderedComponents.erase(comp);
        break;
    default:
        assert(false);
    }
}

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

bool Actor::UpdateOrder::operator()(const ptr& lhs, const ptr& rhs) const noexcept{
    return lhs->getUpdateOrder() < rhs->getUpdateOrder();
}
