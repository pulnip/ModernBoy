#ifndef MODERNBOY_OBJECT_POOL_HPP
#define MODERNBOY_OBJECT_POOL_HPP

#include <cassert>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace ModernBoy
{
    template<typename Object>
    class ObjectPool{
    private:
        using Slot = std::unordered_set<Object>;

        std::vector<Slot> slots;
        std::unordered_set<EntityID> freeSlots;

        EntityID issueID() noexcept{
            [[unlikely]] if(!freeSlots.empty()){
                EntityID new_id = *freeSlots.end();
                freeSlots.erase(new_id);

                return new_id;
            }

            EntityID new_id = slots.size();
            slots.emplace_back(Slot{});

            return new_id;
        }

    public:
        EntityID create(Slot&& new_slot) noexcept{
            EntityID new_id = issueID();
            slots[new_id] = std::move(new_slot);

            return new_id;
        }
        void destroy(EntityID id) noexcept{
            slots[id] = Slot{};
            freeSlots.insert(id);
        }

        Slot& get(EntityID id) noexcept{
            assert(id < slots.size());
            assert(freeSlots.find(id) == freeSlots.end());
            return slots[id];
        }
        const Slot& get(EntityID id) const noexcept{
            assert(id < slots.size());
            assert(freeSlots.find(id) == freeSlots.end());
            return slots[id];
        }

        bool hasComponent(EntityID id, ResourceType t)const noexcept{
            assert(id < slots.size());
            assert(freeSlots.find(id) == freeSlots.cend());
            return slots[id].find(t) != slots[id].cend();
        }

        size_t size() const{ return slots.size() - freeSlots.size(); }
        size_t capacity() const{ return slots.size(); }
    };
} // namespace ModernBoy

#endif // MODERNBOY_OBJECT_POOL_HPP