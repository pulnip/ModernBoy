#include "debug_system.hpp"
#include "entity_registry.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

DebugSystem::DebugSystem(EntityRegistry& registry)
:registry(registry){}

void DebugSystem::update(DeltaTime){
    // for(auto [id, bit, mc, _]: registry.query<Model, Collided>()){
    //     mc.alpha = 0.5f;
    // }
}
// Generator<void> DebugSystem::update(DeltaTime){
//     for(auto [id, bit, mc, _]: registry.query<Mesh, Collided>()){
//         mc.alpha = 0.5f;

//         co_yield 0;
//     }

//     co_return;
// }