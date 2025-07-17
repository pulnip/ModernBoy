#ifndef MODERNBOY_ENTITY_REGISTRY_HPP
#define MODERNBOY_ENTITY_REGISTRY_HPP

#include <ranges>
#include <tuple>
#include <unordered_map>
#ifndef MODERNBOY_TEST
#include "log.hpp"
#endif
#include "util/dynamic_vector.hpp"
#include "game/game_fwd.hpp"
#include "game/component.hpp"

namespace ModernBoy::Game
{
    struct EntityInfo{
        ArchetypeBit bit;
        Index chunkIndex;
    };

    template<typename... Component>
    struct ArchetypeView{
        using Map = std::unordered_map<ArchetypeBit, DynamicVectorV2>;

    private:
        Map&         map;
        ArchetypeBit required_bit;

    public:
        struct Sentinel{};
        struct Iterator{
        private:
            Map::iterator map_it;
            Map::iterator map_end;
            std::size_t   vec_index = 0;
            ArchetypeBit  required_bit;

        public:
            Iterator(Map::iterator map_it, Map::iterator map_end)
            :map_it(map_it), map_end(map_end), required_bit(bits_of<Component...>()){
                advance_to_valid_archetype();
            }

            std::tuple<Component&...> operator*(){
                assert(!at_end());
                auto bit = map_it->first;
                auto& vec = map_it->second;
                assert(vec_index < vec.size());
                auto chunk_ptr = vec[vec_index];

                return std::forward_as_tuple(
                    *static_cast<Component*>(
                        Util::add(chunk_ptr, offset_of<Component>(bit))
                    )...
                );
            }
            Iterator& operator++(){
                auto& vec = map_it->second;
                ++vec_index;
                if(vec_index >= vec.size()){
                    vec_index = 0;
                    ++map_it;
                    advance_to_valid_archetype();
                }
                return *this;
            }
            bool operator==(Sentinel) noexcept{
                return map_it == map_end;
            }
            bool operator!=(Sentinel) noexcept{
                return !((*this)==Sentinel{});
            }

        private:
            void advance_to_valid_archetype(){
                while(map_it != map_end){
                    if( subset(required_bit, map_it->first) &&
                        map_it->second.size() > 0)
                        return;
                    ++map_it;
                }
            }
            bool at_end() const noexcept{ return map_it == map_end; }
        };
        struct ConstIterator{

        };

        ArchetypeView(Map& map):map(map){}

        Iterator begin(){
            return Iterator{map.begin(), map.end()};
        }
        Sentinel end() const noexcept{ return {}; }
        // ConstIterator begin() const;
        // ConstIterator cbegin() const;
        Sentinel cend() const noexcept{ return {}; }
    };

    template<ValueType T>
    void emplace_component(void* chunk, ArchetypeBit bit, T&& t){
        using U = std::remove_cvref_t<T>;

        auto offset = offset_of<U>(bit);
        auto dst = Util::add(chunk, offset);
        *static_cast<U*>(dst) = std::forward<T>(t);
    }
    template<ValueType T1, AllValue... TN>
    void emplace_component(void* chunk, ArchetypeBit bit, T1&& t1, TN&&... tn){
        using U = std::remove_cvref_t<T1>;

        auto offset = offset_of<U>(bit);
        auto dst = Util::add(chunk, offset);
        *static_cast<U*>(dst) = std::forward<T1>(t1);

        emplace_component(chunk, bit, std::forward<TN>(tn)...);
    }

    class EntityRegistry{
    private:
        using ArchetypeMap = std::unordered_map<ArchetypeBit, DynamicVectorV2>;
        using EntityTable = std::unordered_map<EntityID, EntityInfo>;

        ArchetypeMap archetypeMap;
        EntityTable entityTable;

        EntityID id_seed = 0;

    public:
        EntityRegistry() = default;
        ~EntityRegistry() = default;
        EntityRegistry(const EntityRegistry&) = delete;
        EntityRegistry(EntityRegistry&&) = delete;
        EntityRegistry& operator=(const EntityRegistry&) = delete;
        EntityRegistry& operator=(EntityRegistry&&) = delete;

        template<typename... Args>
        EntityID createEntity(Args&&... args){
            auto bit = bits_of<Args...>();

            if(archetypeMap.find(bit) == archetypeMap.end())
                archetypeMap.emplace(bit, size_of(bit));

            auto& vector = archetypeMap.at(bit);
            vector.resize(vector.size() + 1);
            auto index = vector.size() - 1;
            auto chunk = vector[index];

            emplace_component(chunk, bit, std::forward<Args>(args)...);
            // vector.emplace(std::forward<Args>(args)...);

            auto entity_id = issueID();
            entityTable.emplace(entity_id, EntityInfo{
                .bit = bit, .chunkIndex = index
            });
            return entity_id;
        }
        void destroyEntity(EntityID);

        template<typename... Component>
        auto query(){
            return ArchetypeView<Component...>(archetypeMap);
        }
        template<typename Component>
        void appendComponent(EntityID id, Component&& component){
            auto entity_it = entityTable.find(id);
            if(entity_it == entityTable.end()){
#ifndef MODERNBOY_TEST
                GameWarn("Entity {} not exist. component cannot be added", id);
#endif
                return;
            }

            auto& info = entity_it->second;

            if(subset(bit_of<Component>(), info.bit)){
#ifndef MODERNBOY_TEST
                GameWarn("Component {} already exist. (entity: {}, archetype: {})",
                    bit_of<Component>(), id, info.bit);
#endif
                return;
            }

            auto [new_index, old_vec] = moveChunk(info, std::forward<Component>(component));

            updateEntityInfo(info, old_vec,
                info.bit | bit_of<Component>(), new_index
            );
        }
        template<typename Component>
        void removeComponent(EntityID id){
            auto entity_it = entityTable.find(id);
            if(entity_it == entityTable.end()){
#ifndef MODERNBOY_TEST
                GameWarn("Entity {} not exist. component cannot be added", id);
#endif
                return;
            }

            auto& info = entity_it->second;

            if(!subset(bit_of<Component>(), info.bit)){
#ifndef MODERNBOY_TEST
                GameWarn("Component {} not exist. (entity: {}, archetype: {})",
                    bit_of<Component>(), id, info.bit);
#endif
                return;
            }

            // Notice. Allow Entity with no component
            auto new_bit = info.bit & (~bit_of<Component>());

            auto arch_it = archetypeMap.find(info.bit);
            if(arch_it == archetypeMap.end()){
#ifndef MODERNBOY_TEST
                GameWarn("Wrong Archetype: {}", info.bit);
#endif
                return;
            }

            auto& vec = arch_it->second;
            auto chunk = vec[info.chunkIndex];

            // 1. copy and emplace new chunk
            if(archetypeMap.find(new_bit) == archetypeMap.end())
                archetypeMap.emplace(new_bit, size_of(new_bit));
            auto& tgt_vector = archetypeMap.at(new_bit);
            tgt_vector.resize(tgt_vector.size() + 1);
            auto new_index = tgt_vector.size() - 1;
            auto dst = tgt_vector[new_index];

            // copy chunk before component
            Util::chunkcpy(dst, chunk, offset_of<Component>(info.bit));
            // skip target component
            chunk = Util::add(chunk, offset_of<Component>(info.bit) + sizeof(Component));
            // copy chunk after component
            Util::chunkcpy(dst, chunk, size_of(info.bit) - offset_of<Component>(info.bit) - sizeof(Component));

            // 2. remove old chunk
            vec.swap_remove(info.chunkIndex);

            // 3. update swapped entity info
            if(vec.size() > 0){
                auto swapped = findEntityFromProperty(info.bit, vec.size());
                if(swapped == entityTable.end()){
                    throw std::runtime_error("Entity Table integrity Broken!");
                }

                auto& info_swapped = swapped->second;
                info_swapped.chunkIndex = info.chunkIndex;
            }

            // 4. update entity info
            info.bit = new_bit;
            info.chunkIndex = new_index;
        }

    private:
        EntityID issueID();

        DynamicVectorV2& getVector(ArchetypeBit);
        template<typename Component>
        std::tuple<Index, DynamicVectorV2&> moveChunk(EntityInfo& info, Component&& component){
            auto& old_vec = archetypeMap.at(info.bit);
            auto old_index = info.chunkIndex;
            auto chunk = old_vec[old_index];

            auto new_bit = info.bit | bit_of<Component>();
            auto& new_vec = getVector(new_bit);

            new_vec.resize(new_vec.size() + 1);
            auto new_index = new_vec.size() - 1;
            auto dst = new_vec[new_index];

            // 1. copy new chunk
            // copy chunk before component
            Util::chunkcpy(dst, chunk, offset_of<Component>(info.bit));
            chunk = Util::add(        chunk, offset_of<Component>(info.bit));
            // copy component
            Util::chunkcpy(dst, std::forward<Component>(component));
            // copy chunk after component
            Util::chunkcpy(dst, chunk, size_of(info.bit) - offset_of<Component>(info.bit));

            // 2. remove old chunk
            old_vec.swap_remove(info.chunkIndex);

            return {new_index, old_vec};
        }
        void updateEntityInfo(EntityInfo& updated, DynamicVectorV2& swapped,
            ArchetypeBit updated_bit, Index updated_index);

        EntityTable::iterator findEntityFromProperty(ArchetypeBit bit, Index chunkIndex);
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_ENTITY_REGISTRY_HPP