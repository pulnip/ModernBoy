#include <cassert>
#include <iostream>
#include "PubSubMessage.hpp"
#include "GameEngine/IGameEngine.hpp"
#include "SubEngine/SubEngine.hpp"

SubEngine::~SubEngine(){
    if(!owner.expired()){
        notify(Lifetime::DESTRUCTED);
    }
}

void SubEngine::postConstruct() noexcept{
    subscribe(owner);
    notify(Lifetime::CONSTRUCTED);

    injectDependency();
}
