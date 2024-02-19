#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "SubEngine/SoundEngine.hpp"

using namespace Game::SubEngine;

void SoundEngine::postConstruct() noexcept{
    assert(not owner.expired());
    owner.lock()->setSoundEngine(shared_from_this());
}