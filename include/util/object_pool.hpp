#ifndef MODERNBOY_OBJECT_POOL_HPP
#define MODERNBOY_OBJECT_POOL_HPP

#include <cassert>
#include <vector>
#include <unordered_set>

namespace ModernBoy
{
    using SlotIndex = uint32_t;

    template<typename Slot>
    class ObjectPool{
    private:
        using Slots = std::vector<Slot>;
        Slots slots;
        std::unordered_set<SlotIndex> freeSlots;

        SlotIndex issueSlot() noexcept{
            [[unlikely]] if(!freeSlots.empty()){
                SlotIndex free_slotIndex = *freeSlots.end();
                freeSlots.erase(free_slotIndex);

                return free_slotIndex;
            }

            SlotIndex free_slotIndex = slots.size();
            slots.emplace_back(Slot());

            return free_slotIndex;
        }

    public:
        SlotIndex emplace(Slot&& new_slot) noexcept{
            SlotIndex new_index = issueSlot();
            slots[new_index] = std::move(new_slot);

            return new_index;
        }
        SlotIndex push(const Slot& new_slot) noexcept{
            SlotIndex new_index = issueSlot();
            slots[new_index] = new_slot;

            return new_index;
        }
        void erase(SlotIndex index) noexcept{
            slots[index] = Slot{};
            freeSlots.insert(index);
        }

        Slot& get(SlotIndex index) noexcept{
            assert(index < slots.size());
            assert(freeSlots.find(index) == freeSlots.end());
            return slots[index];
        }
        const Slot& get(SlotIndex index) const noexcept{
            assert(index < slots.size());
            assert(freeSlots.find(index) == freeSlots.end());
            return slots[index];
        }
        Slot& operator[](SlotIndex index) noexcept{
            assert(index < slots.size());
            assert(freeSlots.find(index) == freeSlots.end());
            return slots[index];
        }
        const Slot& operator[](SlotIndex index) const noexcept{
            assert(index < slots.size());
            assert(freeSlots.find(index) == freeSlots.end());
            return slots[index];
        }
        Slots get() const{ return slots; }

        size_t size() const{ return slots.size() - freeSlots.size(); }
        size_t capacity() const{ return slots.size(); }
    };
} // namespace ModernBoy

#endif // MODERNBOY_OBJECT_POOL_HPP