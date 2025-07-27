#include <cassert>
#include "script/invoker.hpp"
#include "game/context.hpp"
#include "game/entity_registry.hpp"
#include "game/component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

ActionSystem::ActionSystem(EntityRegistry& registry,
    ::ModernBoy::Script::Invoker& invoker)
:registry(registry), invoker(invoker){}

void ActionSystem::update(DeltaTime dt){
    for(const auto [id, bit, sc]: registry.query<ScriptObject>()){
        if(!sc.isActive)
            continue;
        invoker.invoke(sc.handle, "Update", sc.entity, dt);
    }
}

// Generator<void> ActionSystem::update(DeltaTime dt){
//     for(const auto [id, bit, sc]: registry.query<Script>()){
//         if(!sc.isActive)
//             continue;
//         invoker.invoke(sc.handle, "Update", sc.actor, dt);
//         co_yield 0;
//     }

//     co_return;
// }
