#include "game/entity_registry.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

void EntityRegistry::destroyEntity(EntityID id){
    auto entity_it = entityTable.find(id);
    if(entity_it == entityTable.end()){
#ifndef MODERNBOY_TEST
        GameWarn("Entity {} not exist.", id);
#endif
        return;
    }

    const auto& info = entity_it->second;
    auto arch_it = archetypeMap.find(info.bit);
    if(arch_it == archetypeMap.end()){
#ifndef MODERNBOY_TEST
        GameCritical("Archetype of Entity {}: {}, but ArchetypeVector not exist", id, info.bit);
#endif
        return;
    }

    auto& vec = arch_it->second;
    vec.swap_remove(info.chunkIndex);

    entityTable.erase(id);
}

EntityID EntityRegistry::issueID(){
    return id_seed++;
}