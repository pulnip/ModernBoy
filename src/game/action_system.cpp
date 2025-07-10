#include <cassert>
#include "script/invoker.hpp"
#include "game/context.hpp"
#include "game/action_system.hpp"
#include "game/component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

ActionSystem::ActionSystem(Game::Context& world,
    Script::Invoker& invoker)
:world(world), invoker(invoker){}

Generator<void> ActionSystem::update(DeltaTime){
    for(const auto& [bit, vec]: world.query<ActionComponent>()){
        if(!subset(bit_of<ActionTask>(), bit))
            continue;
        vec.for_each([this](const ActionComponent& ac){
            if(!ac.isActive)
                return;
            invoker.invoke(ac.moduleHandle,
                ac.updateFunc, ac.actor);
        });
        co_yield 0;
    }

    co_return;
}
