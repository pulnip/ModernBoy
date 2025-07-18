#include "game/entity_registry.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

Entity EntityRegistry::query(EntityID id){
    auto entity_it = entityTable.find(id);
    if(entity_it == entityTable.end()){
        GameWarn("Entity {} not exist.", id);
        return {};
    }
    const auto& info = entity_it->second;

    auto arch_it = archetypeMap.find(info.bit);
    if(arch_it == archetypeMap.end()){
        GameCritical("Archetype of Entity {}: {}, but ArchetypeVector not exist", id, info.bit);
        return {};
    }
    auto& vec = arch_it->second;

    return Entity{
        .bit=info.bit,
        .chunk=vec[info.chunkIndex]
    };
}

void EntityRegistry::destroyEntity(EntityID id){
    auto entity_it = entityTable.find(id);
    if(entity_it == entityTable.end()){
        GameWarn("Entity {} not exist.", id);
        return;
    }

    const auto& info = entity_it->second;
    auto arch_it = archetypeMap.find(info.bit);
    if(arch_it == archetypeMap.end()){
        GameCritical("Archetype of Entity {}: {}, but ArchetypeVector not exist", id, info.bit);
        return;
    }

    auto& vec = arch_it->second;
    vec.swap_remove(info.chunkIndex);

    entityTable.erase(id);
}

EntityID EntityRegistry::issueID(){
    return id_seed++;
}

DynamicVectorV2& EntityRegistry::getVector(ArchetypeBit bit){
    auto it = archetypeMap.find(bit);
    if(it != archetypeMap.end())
        return it->second;

    auto[new_it, _] = archetypeMap.try_emplace(bit, size_of(bit));
    return new_it->second;
}

EntityRegistry::EntityTable::iterator
EntityRegistry::findEntityFromProperty(ArchetypeBit bit, Index chunkIndex){
    // TODO. might be replace to entity tag component
    auto it = std::find_if(entityTable.begin(), entityTable.end(),
        [bit, chunkIndex](const auto& pair){
            const EntityInfo& info = pair.second;
            return info.bit == bit && info.chunkIndex == chunkIndex;
        }
    );
    if(it == entityTable.end()){
        GameCritical("Entity with archetype {}, index {} not in entity table!", bit, chunkIndex);
    }
    return it;
}

void EntityRegistry::updateEntityInfo(EntityInfo& updated, DynamicVectorV2& swapped,
    ArchetypeBit updated_bit, Index updated_index
){
    if(swapped.size() > 0){
        auto it = findEntityFromProperty(updated.bit, swapped.size());

        if(it == entityTable.end()){
            throw std::runtime_error("Entity Table integrity Broken!");
        }

        auto& swapped_entity = it->second;
        swapped_entity.chunkIndex = updated.chunkIndex;
    }
    updated.bit = updated_bit;
    updated.chunkIndex = updated_index;
}