#ifndef MODERNBOY_ARCHETYPE_MAP_HPP
#define MODERNBOY_ARCHETYPE_MAP_HPP

#include <atomic>
#include <functional>
#include <unordered_map>
#include "fwd.hpp"
#include "util/dynamic_vector.hpp"
#include "component.hpp"

namespace ModernBoy
{
    class RWPhaseGate{
    private:
        DynamicVector vec;

        std::atomic_uint_fast32_t state = 0;
        static constexpr uint_fast32_t WRITER_BIT
            = 1 << (8*sizeof(uint_fast32_t)-1);
        static constexpr uint_fast32_t READER_MASK
            = WRITER_BIT - 1;

        using Reader = std::function<void(const void*)>;
        using Writer = std::function<void(void*)>;

    public:
        void for_each(Reader fn);
        void transform(Writer fn);
        void transform_range(Writer fn,
            Index start, size_t num);
        

    private:
        void on_read_phase();
        void read_phase_end();
        void on_write_phase();
        void write_phase_end();
    };

    class ArchetypeMap{
    private:
        using ArchetypeIndex = std::pair<ArchetypeBit, Index>;
        std::unordered_map<EntityID, ArchetypeIndex> actor_info;
        std::unordered_map<ArchetypeBit, DynamicVector> archetypeMap;

    public:
        Index insert(ArchetypeBit bit,
            const SparseChunk& chunk);
        DynamicVector& at(ArchetypeBit bit);
        const DynamicVector& at(ArchetypeBit bit) const;

        using iterator = std::unordered_map<ArchetypeBit, DynamicVector>::iterator;
        using const_iterator = std::unordered_map<ArchetypeBit, DynamicVector>::const_iterator;

        iterator begin(){ return archetypeMap.begin(); }
        iterator end(){ return archetypeMap.end(); }
        const_iterator begin() const{ return archetypeMap.begin(); }
        const_iterator end() const{ return archetypeMap.end(); }
        const_iterator cbegin() const{ return archetypeMap.cbegin(); }
        const_iterator cend() const{ return archetypeMap.cend(); }

    };

    void getChunk(TransformComponent* tc,
        CameraComponent* cc, MeshComponent* mc,
        InputComponent* ic, const void* src
    );
}

#endif // MODERNBOY_ARCHETYPE_MAP_HPP