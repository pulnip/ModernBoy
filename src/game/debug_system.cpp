#include "game/debug_system.hpp"
#include "game/entity_registry.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

DebugSystem::DebugSystem(EntityRegistry& registry)
:registry(registry){}

Generator<void> DebugSystem::update(DeltaTime){
    for(auto [id, bit, mc, _]: registry.query<MeshComponent, Collided>()){
        mc.alpha = 0.5f;

        co_yield 0;
    }

    co_return;
}