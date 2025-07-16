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
    struct ComponentInfo{
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
            :map_it(map_it), map_end(map_end), required_bit(bits_of<Component...>()){}

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
                ++vec_index;
                if(map_it != map_end){
                    auto &vec = map_it->second;
                    if(vec_index >= vec.size()){
                        ++map_it;
                        vec_index = 0;
                        advance_to_valid_archetype();
                    }
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

    class EntityRegistry{
    private:
        std::unordered_map<ArchetypeBit, DynamicVectorV2> archetypeMap;
        std::unordered_map<EntityID, ComponentInfo> entityTable;

        EntityID id_seed;

    public:
        EntityRegistry() = default;
        ~EntityRegistry() = default;
        EntityRegistry(const EntityRegistry&) = delete;
        EntityRegistry(EntityRegistry&&) = delete;
        EntityRegistry& operator=(const EntityRegistry&) = delete;
        EntityRegistry& operator=(EntityRegistry&&) = delete;

        template<typename... Args>
        void createEntity(ArchetypeBit bit, Args&&... args){
            if(archetypeMap.find(bit) == archetypeMap.end())
                archetypeMap.emplace(bit, size_of(bit));

            auto& vector = archetypeMap.at(bit);
            vector.emplace(std::forward<Args>(args)...);

            entityTable.emplace(issueID(), ComponentInfo{
                .bit = bit, .chunkIndex = vector.size()-1
            });
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
                GameWarn("Component {} already exist. (entity: {}, archetype: {})",
                    bit_of<Component>(), id, info.bit);
                return;
            }

            auto new_bit = info.bit | bit_of<Component>();

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
            auto new_index = tgt_vector.size();
            auto dst = tgt_vector[new_index];
            tgt_vector.resize(new_index + 1);

            // copy chunk before component
            dst = Util::chunkcpy(dst, chunk, offset_of<Component>(info.bit));
            chunk = Util::add(        chunk, offset_of<Component>(info.bit));
            // copy component
            dst = Util::chunkcpy(dst, std::forward<Component>(component));
            // copy chunk after component
            dst = Util::chunkcpy(dst, chunk, size_of(info.bit) - offset_of<Component>(info.bit));

            // 2. remove old chunk
            archetypeMap.at(info.bit).swap_remove(info.chunkIndex);

            // 3. update entity info
            info.bit = new_bit;
            info.chunkIndex = new_index;
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
                GameWarn("Component {} not exist. (entity: {}, archetype: {})",
                    bit_of<Component>(), id, info.bit);
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
            auto new_index = tgt_vector.size();
            auto dst = tgt_vector[new_index];
            tgt_vector.resize(new_index + 1);
    
            // copy chunk before component
            dst = Util::chunkcpy(dst, chunk, offset_of<Component>(info.bit));
            // skip target component
            chunk = Util::add(chunk, offset_of<Component>(info.bit) + sizeof(Component));
            // copy chunk after component
            dst = Util::chunkcpy(dst, chunk, size_of(info.bit) - offset_of<Component>(info.bit) - sizeof(Component));

            // 2. remove old chunk
            archetypeMap.at(info.bit).swap_remove(info.chunkIndex);

            // 3. update entity info
            info.bit = new_bit;
            info.chunkIndex = new_index;
        }

    private:
        EntityID issueID();
    };
} // namespace ModernBoy::Game

#endif // MODERNBOY_ENTITY_REGISTRY_HPP