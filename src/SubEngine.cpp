#include <cassert>

#include "PubSubMessage.hpp"
#include "GameEngine/IGameEngine.hpp"
#include "SubEngine/SubEngine.hpp"

void SubEngine::postConstruct() noexcept{
    subscribe(owner);
    notify(Lifetime::CONSTRUCTED);

    injectDependency();
}
