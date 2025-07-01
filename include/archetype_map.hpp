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

        mutable std::atomic_uint_fast32_t state = 0;
        static constexpr uint_fast32_t WRITER_BIT
            = 1 << (8*sizeof(uint_fast32_t)-1);
        static constexpr uint_fast32_t READER_MASK
            = WRITER_BIT - 1;

        using Reader = std::function<void(const void*)>;
        using Writer = std::function<void(void*)>;

    public:
        template<typename... Args>
        RWPhaseGate(Args&&... args)
        :vec(std::forward<Args>(args)...){}

        void for_each(Reader fn) const;
        void transform(Writer fn);
        template<typename R>
        R mutate(std::function<R(DynamicVector&)> fn){
            on_write_phase();
            auto ret = fn(vec);
            write_phase_end();
            return ret;
        }

        size_t size() const;
        void get(Index i, void* dst) const;
        void free(Index i);

    private:
        void* operator[](Index index);
        const void* operator[](Index index) const;

        void on_read_phase() const;
        void read_phase_end() const;
        void on_write_phase();
        void write_phase_end();

        friend class ArchetypeMap;
    };

    class ArchetypeMap{
    private:
        using ArchetypeIndex = std::pair<ArchetypeBit, Index>;
        std::unordered_map<EntityID, ArchetypeIndex> actor_info;
        std::unordered_map<ArchetypeBit, RWPhaseGate> archetypeMap;

    public:
        Index insert(ArchetypeBit bit,
            const SparseChunk& chunk);
        RWPhaseGate& at(ArchetypeBit bit);
        const RWPhaseGate& at(ArchetypeBit bit) const;

        using iterator = std::unordered_map<ArchetypeBit, RWPhaseGate>::iterator;
        using const_iterator = std::unordered_map<ArchetypeBit, RWPhaseGate>::const_iterator;

        iterator begin(){ return archetypeMap.begin(); }
        iterator end(){ return archetypeMap.end(); }
        const_iterator begin() const{ return archetypeMap.begin(); }
        const_iterator end() const{ return archetypeMap.end(); }
        const_iterator cbegin() const{ return archetypeMap.cbegin(); }
        const_iterator cend() const{ return archetypeMap.cend(); }

        TransformComponent getTransformComponent(ArchetypeBit bit, Index index);
        CameraComponent getCameraComponent(ArchetypeBit bit, Index index);
        MeshComponent getMeshComponent(ArchetypeBit bit, Index index);
        InputComponent getInputComponent(ArchetypeBit bit, Index index);
        void setTransformComponent(const TransformComponent& component,
            ArchetypeBit bit, Index index);
        void setCameraComponent(const CameraComponent& component,
            ArchetypeBit bit, Index index);
        void setMeshComponent(const MeshComponent& component,
            ArchetypeBit bit, Index index);
        void setInputComponent(const InputComponent& component,
            ArchetypeBit bit, Index index);
    };

    // professional only
    void getChunk(TransformComponent* tc,
        CameraComponent* cc, MeshComponent* mc,
        InputComponent* ic, const void* src,
        ArchetypeBit bit
    );
}

#endif // MODERNBOY_ARCHETYPE_MAP_HPP