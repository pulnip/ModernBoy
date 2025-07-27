#ifndef MODERNBOY_ENTITY_REGISTRY_HPP
#define MODERNBOY_ENTITY_REGISTRY_HPP

#include <ranges>
#include <tuple>
#include <unordered_map>
#include "log.hpp"
#include "util/dynamic_vector.hpp"
#include "game/game_fwd.hpp"
#include "game/component.hpp"

namespace ModernBoy::Game
{
    struct EntityInfo{
        ArchetypeBit bit;
        Index chunkIndex;
    };

    template<typename... Ts>
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
            :map_it(map_it), map_end(map_end), required_bit(bits_of<Ts...>()){
                advance_to_valid_archetype();
            }

            std::tuple<EntityID, ArchetypeBit, Ts&...> operator*(){
                assert(!at_end());
                auto bit = map_it->first;
                auto& vec = map_it->second;
                assert(vec_index < vec.size());
                auto chunk_ptr = vec[vec_index];

                return std::forward_as_tuple(
                    *static_cast<EntityID*>(chunk_ptr),
                    map_it->first,
                    *static_cast<Ts*>(
                        Util::add(chunk_ptr, offset_of<Ts>(bit))
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

    struct Entity{
        Game::ArchetypeBit bit = 0;
        void* chunk = nullptr;
    };

    template<ValueType T>
    void emplace_component(EntityID id, void* chunk, ArchetypeBit bit, T&& t){
        using U = std::remove_cvref_t<T>;

        auto offset = offset_of<U>(bit);
        auto dst = Util::add(chunk, offset);
        *static_cast<U*>(dst) = std::forward<T>(t);
        static_cast<U*>(dst)->entity = id;
    }
    template<ValueType T1, AllValue... TN>
    void emplace_component(EntityID id, void* chunk, ArchetypeBit bit,
        T1&& t1, TN&&... tn
    ){
        using U = std::remove_cvref_t<T1>;

        auto offset = offset_of<U>(bit);
        auto dst = Util::add(chunk, offset);
        *static_cast<U*>(dst) = std::forward<T1>(t1);
        static_cast<U*>(dst)->entity = id;

        emplace_component(id, chunk, bit, std::forward<TN>(tn)...);
    }
    template<PointerType T>
    void emplace_component(EntityID id, void* chunk, ArchetypeBit bit, const T t){
        using U = std::remove_pointer_t<std::remove_cvref_t<T>>;

        auto offset = offset_of<U>(bit);
        auto dst = Util::add(chunk, offset);
        *static_cast<U>(dst) = *t;
        static_cast<U>(dst)->entity = id;
    }
    template<PointerType T1, AllPointer... TN>
    void emplace_component(EntityID id, void* chunk, ArchetypeBit bit,
        const T1 t1, const TN... tn
    ){
        using U = std::remove_pointer_t<std::remove_cvref_t<T1>>;

        auto offset = offset_of<U>(bit);
        auto dst = Util::add(chunk, offset);
        *static_cast<U>(dst) = *t1;
        static_cast<U>(dst)->entity = id;

        emplace_component(id, chunk, bit, tn...);
    }
    template<OptionalType T>
    void emplace_component(EntityID id, void* chunk, ArchetypeBit bit, T&& t){
        using U = remove_optional_t<std::remove_cvref_t<T>>;

        if(t.has_value()){
            auto offset = offset_of<U>(bit);
            auto dst = Util::add(chunk, offset);
            *static_cast<U*>(dst) = t.value();
            static_cast<U*>(dst)->entity = id;
        }
    }
    template<OptionalType T1, AllOptional... TN>
    void emplace_component(EntityID id, void* chunk, ArchetypeBit bit,
        const T1 t1, const TN... tn
    ){
        using U = remove_optional_t<std::remove_cvref_t<T1>>;

        if(t1.has_value()){
            auto offset = offset_of<U>(bit);
            auto dst = Util::add(chunk, offset);
            *static_cast<U*>(dst) = t1.value();
            static_cast<U*>(dst)->entity = id;
        }

        emplace_component(id, chunk, bit, tn...);
    }

    template<ValueType T>
    ArchetypeBit bits_of(T){
        using U = std::remove_cvref_t<T>;

        return bit_of<U>();
    }
    template<ValueType T1, AllValue... TN>
    ArchetypeBit bits_of(T1, TN... tn){
        using U = std::remove_cvref_t<T1>;

        return bit_of<U>() + bits_of(tn...);
    }

    template<PointerType T>
    ArchetypeBit bits_of(T t){
        using U = std::remove_pointer_t<std::remove_cvref_t<T>>;

        return t != nullptr ? bit_of<U>() : 0;
    }
    template<PointerType T1, AllPointer... TN>
    ArchetypeBit bits_of(T1 t1, TN... tn){
        using U = std::remove_pointer_t<std::remove_cvref_t<T1>>;
        auto bit = bits_of(tn...);

        return bit + (t1 != nullptr ? bit_of<U>() : 0);
    }

    template<OptionalType T>
    ArchetypeBit bits_of(const T& t){
        using U = remove_optional_t<std::remove_cvref_t<T>>;

        return t.has_value() ? bit_of<U>() : 0;
    }
    template<OptionalType T1, AllOptional... TN>
    ArchetypeBit bits_of(const T1& t1, const TN&... tn){
        using U = remove_optional_t<std::remove_cvref_t<T1>>;
        auto bit = bits_of(tn...);

        return bit + (t1.has_value() ? bit_of<U>() : 0);
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
        ~EntityRegistry();
        EntityRegistry(const EntityRegistry&) = delete;
        EntityRegistry(EntityRegistry&&) = delete;
        EntityRegistry& operator=(const EntityRegistry&) = delete;
        EntityRegistry& operator=(EntityRegistry&&) = delete;

        template<typename... Args>
        EntityID createEntity(Args&&... args){
            auto bit = bits_of(args...);
            // auto bit = bits_of<remove_optional_t<std::remove_cvref_t<Args>>...>();

            if(archetypeMap.find(bit) == archetypeMap.end())
                archetypeMap.emplace(bit, size_of(bit));

            auto& vector = archetypeMap.at(bit);
            vector.resize(vector.size() + 1);
            auto index = vector.size() - 1;
            auto chunk = vector[index];

            auto entity_id = issueID();
            entityTable.emplace(entity_id, EntityInfo{
                .bit = bit, .chunkIndex = index
            });
            *static_cast<EntityID*>(chunk) = entity_id;
            emplace_component(entity_id, chunk, bit, std::forward<Args>(args)...);

            return entity_id;
        }
        void destroyEntity(EntityID);

        template<typename... Ts>
        auto query(){
            return ArchetypeView<Ts...>(archetypeMap);
        }
        template<typename... Ts>
        std::tuple<Ts&...> query(EntityID id){
            const auto& info = entityTable.at(id);
            auto& vec = archetypeMap.at(info.bit);
            auto chunk = vec[info.chunkIndex];

            return std::forward_as_tuple(
                *static_cast<Ts*>(
                    Util::add(chunk, offset_of<Ts>(info.bit))
                )...
            );
        }
        template<typename T>
        std::pair<T&, bool> query_safe(EntityID id){
            const auto& info = entityTable.at(id);
            auto& vec = archetypeMap.at(info.bit);
            auto chunk = vec[info.chunkIndex];

            auto offset = offset_of<T>(info.bit);
            return {
                *static_cast<T*>(Util::add(chunk, offset)),
                offset != std::numeric_limits<size_t>::max()
            };
        }
        Entity query(EntityID id);

        template<typename T>
        void appendComponent(EntityID id, T&& component){
            auto entity_it = entityTable.find(id);
            if(entity_it == entityTable.end()){
                GameWarn("Entity {} not exist. component cannot be added", id);
                return;
            }

            auto& info = entity_it->second;

            if(subset(bit_of<T>(), info.bit)){
                GameWarn("Component {} already exist. (entity: {}, archetype: {})",
                    bit_of<T>(), id, info.bit);
                return;
            }

            auto [new_index, old_vec] = moveChunk(info,
                std::forward<T>(component));
            updateEntityInfo(info, old_vec,
                info.bit | bit_of<T>(), new_index
            );
        }
        template<typename T>
        void removeComponent(EntityID id){
            auto entity_it = entityTable.find(id);
            if(entity_it == entityTable.end()){
                GameWarn("Entity {} not exist. component cannot be added", id);
                return;
            }

            auto& info = entity_it->second;

            if(!subset(bit_of<T>(), info.bit)){
                GameWarn("{} not exist. (entity: {}, archetype: {})",
                    name_of<T>(), id, info.bit);
                return;
            }

            auto [new_index, old_vec] = moveChunk<T>(info);
            updateEntityInfo(info, old_vec,
                info.bit & (~bit_of<T>()), new_index);
        }

    private:
        EntityID issueID();

        DynamicVectorV2& getVector(ArchetypeBit);

        template<typename T>
        std::tuple<Index, DynamicVectorV2&> moveChunk(EntityInfo& info, T&& component){
            auto& old_vec = archetypeMap.at(info.bit);
            auto old_index = info.chunkIndex;
            auto chunk = old_vec[old_index];

            auto new_bit = info.bit | bit_of<T>();
            auto& new_vec = getVector(new_bit);

            new_vec.resize(new_vec.size() + 1);
            auto new_index = new_vec.size() - 1;
            auto dst = new_vec[new_index];

            // 1. copy new chunk
            // copy chunk before component
            dst = Util::chunkcpy(dst, chunk, offset_of<T>(new_bit));
            chunk = Util::add(        chunk, offset_of<T>(new_bit));
            // copy component
            dst = Util::chunkcpy(dst, std::forward<T>(component));
            // copy chunk after component
            Util::chunkcpy(dst, chunk, size_of(info.bit) - offset_of<T>(new_bit));

            // 2. remove old chunk
            old_vec.swap_remove(info.chunkIndex);

            return {new_index, old_vec};
        }
        template<typename T>
        std::tuple<Index, DynamicVectorV2&> moveChunk(EntityInfo& info){
            auto& old_vec = archetypeMap.at(info.bit);
            auto old_index = info.chunkIndex;
            auto chunk = old_vec[old_index];

            auto new_bit = info.bit & (~bit_of<T>());
            auto& new_vec = getVector(new_bit);

            new_vec.resize(new_vec.size() + 1);
            auto new_index = new_vec.size() - 1;
            auto dst = new_vec[new_index];

            // 1. copy new chunk
            // copy chunk before component
            dst = Util::chunkcpy(dst, chunk, offset_of<T>(info.bit));
            // skip target component
            chunk = Util::add(chunk, offset_of<T>(info.bit) + sizeof(T));
            // copy chunk after component
            Util::chunkcpy(dst, chunk, size_of(info.bit) - offset_of<T>(info.bit) - sizeof(T));

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