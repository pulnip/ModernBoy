#include <cassert>

#include "PubSubMessage.hpp"
#include "SubEngine/ActorManager.hpp"
#include "Actor/Actor.hpp"
#include "Component/Component.hpp"

void Actor::postConstruct() noexcept {
    subscribe(owner);
    notify(Lifetime::CONSTRUCTED);
    
    injectDependency();
}

void Actor::update(const float& deltaTime) noexcept {
    if (state != State::EActive) return;

    for (auto& comp: orderedComponents) {
        comp->update(deltaTime);
    }
    updateActor(deltaTime);
}

std::optional<std::weak_ptr<IComponent>>
Actor::queryComponent(const ComponentName name) noexcept {
    auto result = components.find(name);
    if (result == components.end()) {
        return std::nullopt;
    }
    return result->second;
}

void Actor::onNotify(MSG_t msg, spObservable comp) noexcept {
    switch (msg) {
    case Lifetime::CONSTRUCTED:
        components.emplace(comp->getName(), comp);
        orderedComponents.emplace(comp);
        break;
    case Lifetime::DESTRUCTED:
        components.erase(comp->getName());
        break;
    default:
        assert(false);
    }
}

bool Actor::UpdateOrder::operator()(const ptr& lhs, const ptr& rhs) const noexcept{
    return lhs->getUpdateOrder() < rhs->getUpdateOrder();
}
