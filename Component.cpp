#include "Component.hpp"
#include "Actor.hpp"
#include "PubSubMessage.hpp"

Component::Component(const std::weak_ptr<Actor> owner) noexcept : owner(owner) {
    subscribe(owner);
}

void Component::postConstruct() noexcept {
    notify(Lifetime::CONSTRUCTED);
}
