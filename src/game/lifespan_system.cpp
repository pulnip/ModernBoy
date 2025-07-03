#include "game/lifespan_system.hpp"
#include "task.hpp"
#include "app_state.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

LifespanSystem::LifespanSystem(AppState& app)
:app(app){}

size_t LifespanSystem::yield_count() const noexcept{
    size_t numTask = 0;

    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<LifeSpanComponent>(), bit))
            continue;
        vec.for_each([&numTask](const LifeSpanComponent& lc){
            if(!lc.isAlive)
                numTask += 1;
        });
    }

    numDeadActors = numTask;
    return numTask;
}

Generator<void> LifespanSystem::updateTask(DeltaTime) noexcept{
    std::vector<EntityID> deadActors;
    deadActors.reserve(numDeadActors);

    // remove actor from current epoch
    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<LifeSpanComponent>(), bit))
            continue;
        vec.for_each([&deadActors](const LifeSpanComponent& lc){
            if(!lc.isAlive)
                deadActors.emplace_back(lc.actor);
        });
    }
    for(const auto& deadActor: deadActors){
        app.destroyActor(deadActor);
        co_yield 0;
    }

    co_return;
}

Generator<void> LifespanSystem::update(DeltaTime) noexcept{
    std::vector<EntityID> deadActors;
    deadActors.reserve(numDeadActors);

    // remove actor from next epoch
    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<LifeSpanComponent>(), bit))
            continue;
        vec.for_each([&deadActors](const LifeSpanComponent& lc){
            if(!lc.isAlive)
                deadActors.emplace_back(lc.actor);
        });
    }
    for(const auto& deadActor: deadActors){
        app.destroyActor(deadActor);
    
        co_yield 0;
    }

    co_return;
}
