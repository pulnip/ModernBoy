#ifndef MODERNBOY_GAME_ARCHETYPE_MAP_HPP
#define MODERNBOY_GAME_ARCHETYPE_MAP_HPP

#include <atomic>
#include <functional>
#include <unordered_map>
#include "util/dynamic_vector.hpp"
#include "util/type_traits.hpp"
#include "game/game_fwd.hpp"
#include "game/component.hpp"

namespace ModernBoy::Game
{
    class RWPhaseGate{
    private:
        ArchetypeBit bit;
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
        RWPhaseGate(ArchetypeBit bit, Args&&... args)
        :bit(bit), vec(std::forward<Args>(args)...){}

        template<typename Fn>
        void for_each(Fn&& fn) const{
            using args = fn_args_t<Fn>;
            using plain_args = fn_decayed_args_t<Fn>;

            // on_read_phase();
            for(const auto& chunk: vec)
                [&]<std::size_t... I>(std::index_sequence<I...>){
                    fn(chunk.template at<std::tuple_element_t<I, plain_args>>(
                        offset_of<std::tuple_element_t<I, plain_args>>(bit))...);
                }(std::make_index_sequence<std::tuple_size_v<args>>{});
            // read_phase_end();
        }
        void transform(Writer fn);
        template<typename R>
        R mutate(std::function<R(DynamicVector&)> fn){
            // on_write_phase();
            auto ret = fn(vec);
            // write_phase_end();
            return ret;
        }

        void update(Index index, Writer fn);

        template<typename Component>
        const Component& get(Index index) const{
            return vec.at<Component>(
                index, offset_of<Component>(bit)
            );
        }
        template<typename Component>
        Component& get(Index index){
            return vec.at<Component>(
                index, offset_of<Component>(bit)
            );
        }
        template<typename Component>
        ABNORMAL_FLAG set(Index index, Component&& component){
            auto& comp = vec.at<Component>(
                index, offset_of<Component>(bit));
            comp = std::forward<Component>(component);
            return false;
        }

        size_t size() const;
        void copy(Index i, void* dst) const;
        void* get(Index i);
        const void* get(Index i) const;
        void free(Index i);

        DynamicVector& raw();
        const DynamicVector& raw() const;

        void on_read_phase() const;
        void read_phase_end() const;
        void on_write_phase();
        void write_phase_end();

    private:
        void* operator[](Index index);
        const void* operator[](Index index) const;

        friend class ArchetypeMap;
    };

    class ArchetypeMap{
    private:
        using ArchetypeIndex = std::pair<ArchetypeBit, Index>;
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

        template<typename Component>
        Component get(ArchetypeBit bit, Index index){
            const auto& vec = archetypeMap.at(bit);
            // vec.on_read_phase();
            auto component = vec.get<Component>(index);
            // vec.read_phase_end();
            return component;
        }
        template<typename Component>
        ABNORMAL_FLAG set(Component&& component,
            ArchetypeBit bit, Index index
        ){
            auto& vec = archetypeMap.at(bit);
            // vec.on_write_phase();
            vec.set<Component>(
                index, std::forward<Component>(component));
            // vec.write_phase_end();
            return false;
        }
        template<typename Component>
        Index pop(ArchetypeBit bit, Index index){
            auto& vec = archetypeMap.at(bit);
            auto src = vec[index];
            auto component = vec.get<Component>(index);

            auto new_bit = bit & (!bit_of<Component>());
            Index new_index = 0;
            auto new_size = size_of(new_bit);
            if(new_bit == 0)
                return new_index;
            
            if(archetypeMap.find(new_bit) == archetypeMap.end()){
                auto [it, ret] = archetypeMap.try_emplace(
                    new_bit, new_bit, new_size);
            }

            auto& vector = archetypeMap.at(new_bit);
            new_index = vector.template mutate<Index>([bit, src](DynamicVector& vec){
                auto new_index = vec.insertRange(1);
                auto dst = vec[new_index];

                dst = Util::chunkcpy(dst, src, offset_of<Component>(bit));
                src = Util::add(src, offset_of<Component>(bit)+sizeof(Component));
                dst = Util::chunkcpy(dst, src, size_of(bit)-offset_of<Component>(bit)-sizeof(Component));

                return new_index;
            });

            return new_index;
        }
        template<typename Component>
        Index push(Component&& component,
            ArchetypeBit bit, Index index
        ){
            auto& vec = archetypeMap.at(bit);
            auto src = vec[index];

            auto new_bit = bit | bit_of<Component>();
            auto new_size = size_of(new_bit);

            if(archetypeMap.find(new_bit) == archetypeMap.end()){
                auto [it, ret] = archetypeMap.try_emplace(
                    new_bit, new_bit, new_size);
            }

            auto& vector = archetypeMap.at(new_bit);
            Index new_index = vector.template mutate<Index>([bit, src, &component](DynamicVector& vec){
                auto new_index = vec.insertRange(1);
                auto dst = vec[new_index];

                dst = Util::chunkcpy(dst, src, offset_of<Component>(bit));
                dst = Util::chunkcpy(dst, component);
                src = Util::add(src, offset_of<Component>(bit));
                dst = Util::chunkcpy(dst, src, size_of(bit)-offset_of<Component>(bit));

                return new_index;
            });

            return new_index;
        }
    }; static_assert(std::ranges::range<ArchetypeMap>);

    ArchetypeBit archetype(const TransformComponent*,
        const CameraComponent*, const MeshComponent*,
        const InputComponent*
    );
} // namespace ModernBoy::Game

#endif // MODERNBOY_GAME_ARCHETYPE_MAP_HPP
