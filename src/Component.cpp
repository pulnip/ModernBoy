#include <cassert>

#include "PubSubMessage.hpp"
#include "Actor/IActor.hpp"
#include "Component/Component.hpp"

Component::~Component(){
    if(owner.expired()){
        notify(Lifetime::DESTRUCTED);
    }
}

void Component::postConstruct() noexcept {
    subscribe(owner);
    notify(Lifetime::CONSTRUCTED);

    updateOrder=initUpdateOrder();

    injectDependency();
}
