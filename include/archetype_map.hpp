#ifndef MODERNBOY_ARCHETYPE_MAP_HPP
#define MODERNBOY_ARCHETYPE_MAP_HPP

#include <unordered_map>
#include "fwd.hpp"
#include "util/dynamic_vector.hpp"
#include "component.hpp"

namespace ModernBoy
{
    class ArchetypeMap{
    private:
        using ArchetypeIndex = std::pair<ArchetypeBit, Index>;
        std::unordered_map<EntityID, ArchetypeIndex> actor_info;
        std::unordered_map<ArchetypeBit, DynamicVector> archetypeMap;

    public:
        using iterator = std::unordered_map<ArchetypeBit, DynamicVector>::iterator;
        using const_iterator = std::unordered_map<ArchetypeBit, DynamicVector>::const_iterator;

        iterator begin(){ return archetypeMap.begin(); }
        iterator end(){ return archetypeMap.end(); }
        const_iterator begin() const{ return archetypeMap.begin(); }
        const_iterator end() const{ return archetypeMap.end(); }
        const_iterator cbegin() const{ return archetypeMap.cbegin(); }
        const_iterator cend() const{ return archetypeMap.cend(); }

        Index insert(ArchetypeBit bit,
            const SparseChunk& chunk);
    };

    void getChunk(TransformComponent* tc,
        CameraComponent* cc, MeshComponent* mc,
        InputComponent* ic, const void* src
    );
}

#endif // MODERNBOY_ARCHETYPE_MAP_HPP