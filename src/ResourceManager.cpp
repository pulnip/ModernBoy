#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "SubEngine/ResourceManager.hpp"

using namespace Game::SubEngine;

void ResourceManager::postConstruct() noexcept{
    assert(not owner.expired());
    owner.lock()->setResourceManager(shared_from_this());
}