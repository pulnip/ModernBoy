#include "game/lifespan_system.hpp"
#include "engine/task.hpp"
#include "game/entity_registry.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

LifespanSystem::LifespanSystem(EntityRegistry& registry)
:registry(registry){}

void LifespanSystem::updateTask(DeltaTime) noexcept{
    std::vector<EntityID> deadActors;
    deadActors.reserve(numDeadActors);

    // remove actor from current epoch
    for(const auto [id, bit, lc]: registry.query<LifeSpan>()){
        if(!lc.isAlive)
            deadActors.emplace_back(id);
    }
    for(const auto& deadActor: deadActors){
        registry.destroyEntity(deadActor);
    }
}

void LifespanSystem::update(DeltaTime) noexcept{
    std::vector<EntityID> deadActors;
    deadActors.reserve(numDeadActors);

    // remove actor from next epoch
    for(const auto [id, bit, lc]: registry.query<LifeSpan>()){
        if(!lc.isAlive)
            deadActors.emplace_back(id);
    }
    for(const auto& deadActor: deadActors){
        registry.destroyEntity(deadActor);
    }
}

// size_t LifespanSystem::yield_count() const noexcept{
//     size_t numTask = 0;

//     for(const auto [id, bit, lc]: registry.query<LifeSpan>()){
//         if(!lc.isAlive)
//             numTask += 1;
//     }

//     numDeadActors = numTask;
//     return numTask;
// }

// Generator<void> LifespanSystem::updateTask(DeltaTime) noexcept{
//     std::vector<EntityID> deadActors;
//     deadActors.reserve(numDeadActors);

//     // remove actor from current epoch
//     for(const auto [id, bit, lc]: registry.query<LifeSpan>()){
//         if(!lc.isAlive)
//             deadActors.emplace_back(id);
//     }
//     for(const auto& deadActor: deadActors){
//         registry.destroyEntity(deadActor);
//         co_yield 0;
//     }

//     co_return;
// }

// Generator<void> LifespanSystem::update(DeltaTime) noexcept{
//     std::vector<EntityID> deadActors;
//     deadActors.reserve(numDeadActors);

//     // remove actor from next epoch
//     for(const auto [id, bit, lc]: registry.query<LifeSpan>()){
//         if(!lc.isAlive)
//             deadActors.emplace_back(id);
//     }
//     for(const auto& deadActor: deadActors){
//         registry.destroyEntity(deadActor);
    
//         co_yield 0;
//     }

//     co_return;
// }
