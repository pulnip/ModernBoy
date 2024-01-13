#include <cassert>
#include <ranges>

#include "Actor.hpp"
#include "ActorManager.hpp"
#include "Component.hpp"
#include "PubSubMessage.hpp"

void Actor::processInput(const uint8_t *keyState) noexcept {
    if (state != EActive)
        return;

    // Alternative
    // for(auto &[_, comp] : components) {
    for (auto &comp : components | std::views::values) {
        comp->processInput(keyState);
    }
    processActorInput(keyState);
}

void Actor::update(const float &deltaTime) noexcept {
    if (state != EActive)
        return;

    updateComponents(deltaTime);
    updateActor(deltaTime);
}

void Actor::onNotify(MSG_t msg, spObservable comp) noexcept {
    switch (msg) {
    case Lifetime::CONSTRUCTED:
        appendComponent(comp);
        break;
    case Lifetime::DESTRUCTED:
        components.erase(comp->getName());
        break;
    default:
        assert(false);
    }
}

void Actor::appendComponent(const std::shared_ptr<Component> component) noexcept {
    components.emplace(component->getName(), component);
    orderedComponents.emplace(component);
}
std::optional<std::weak_ptr<Component>>
Actor::queryComponent(const ComponentName name) noexcept {
    auto result = components.find(name);
    if (result == components.end()) {
        return std::nullopt;
    }
    return result->second;
}

Actor::Actor(const std::weak_ptr<ActorManager> owner) noexcept : owner(owner) {
    subscribe(owner);
}

void Actor::postConstruct() noexcept {
    notify(Lifetime::CONSTRUCTED);
}

void Actor::updateComponents(const float &deltaTime) noexcept {
    for (auto &comp : orderedComponents) {
        comp->update(deltaTime);
    }
}

bool Actor::UpdateOrder::operator()(const ptr &lhs, const ptr &rhs) const {
    return lhs->getUpdateOrder() < rhs->getUpdateOrder();
}
