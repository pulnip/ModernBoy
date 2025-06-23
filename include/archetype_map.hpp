#ifndef MODERNBOY_ARCHETYPE_MAP_HPP
#define MODERNBOY_ARCHETYPE_MAP_HPP

#include <unordered_map>
#include "fwd.hpp"
#include "util/dynamic_vector.hpp"

namespace ModernBoy
{
    class ArchetypeMap{
    private:
        using ArchetypeIndex = std::pair<ArchetypeBit, Index>;
        std::unordered_map<EntityID, ArchetypeIndex> actor_info;
        std::unordered_map<ArchetypeBit, DynamicVector> archetypeMap;

    public:
        void add(EntityID actor, ArchetypeBit bit,
            const SparseChunk& chunk);
    };
}

#endif // MODERNBOY_ARCHETYPE_MAP_HPP