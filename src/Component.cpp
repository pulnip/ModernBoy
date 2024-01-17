#include "Component/Component.hpp"
#include "Actor/Actor.hpp"
#include "PubSubMessage.hpp"

void Component::postConstruct() noexcept {
    subscribe(owner);
    notify(Lifetime::CONSTRUCTED);

    injectDependency();
}
