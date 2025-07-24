#ifndef MODERNBOY_OBJECT_POOL_HPP
#define MODERNBOY_OBJECT_POOL_HPP

#include <algorithm>
#include <cassert>
#include <new>
#include <print>
#include <ranges>
#include <unordered_set>
#include <vector>
#include "common/alias.hpp"
#include "common/type.hpp"

namespace ModernBoy
{
    template<typename T>
    class ObjectPool{
    private:
        using Ts = std::vector<T>;
        Ts pool;
        std::unordered_set<Index> freeIndexes;

        Index issueSlot() noexcept{
            [[unlikely]] if(!freeIndexes.empty()){
                Index freeIndex = *freeIndexes.begin();
                freeIndexes.erase(freeIndex);

                return freeIndex;
            }

            Index freeIndex = pool.size();
            pool.resize(pool.size() + 1);

            return freeIndex;
        }

    public:
        void resize(size_t count){
            assert(size() <= count);
            freeIndexes.reserve(freeIndexes.size() + count-size());
            for(size_t i=size(); i<count; ++i){
                freeIndexes.emplace(i);
            }
            pool.resize(count);
        }
        void reserve(size_t count){
            assert(size() <= count);
            pool.resize(count);
        }

        Index newIndex() noexcept{
            Index new_index = issueSlot();
            return new_index;
        }
        Index emplace(T&& x) noexcept{
            Index new_index = issueSlot();
            pool[new_index] = std::move(x);

            return new_index;
        }
        template<typename Range>
        std::vector<Index> append_range(Range&& ranges) noexcept{
            size_t a_size = ranges.size();
            std::vector<Index> allocatedIndex(a_size);
            resize(size() + a_size);
            size_t i=0;
            for(auto&& x: ranges){
                allocatedIndex[i++] = emplace(std::forward(x));
            }

            return allocatedIndex;
        }

        Index push(const T& x) noexcept{
            Index new_index = issueSlot();
            pool[new_index] = x;

            return new_index;
        }
        void erase(Index idx) noexcept{
            freeIndexes.insert(idx);
        }

        T& get(Index idx) noexcept{
            assert(idx < pool.size());
            assert(freeIndexes.find(idx) == freeIndexes.end());
            return pool[idx];
        }
        const T& get(Index idx) const noexcept{
            assert(idx < pool.size());
            assert(freeIndexes.find(idx) == freeIndexes.end());
            return pool[idx];
        }
        T& operator[](Index idx) noexcept{
            assert(idx < pool.size());
            assert(freeIndexes.find(idx) == freeIndexes.end());
            return pool[idx];
        }
        const T& operator[](Index idx) const noexcept{
            assert(idx < pool.size());
            assert(freeIndexes.find(idx) == freeIndexes.end());
            return pool[idx];
        }
        Ts get() const{ return pool; }

        size_t size() const{ return pool.size() - freeIndexes.size(); }
        size_t capacity() const{ return pool.size(); }
    };

    template<typename T>
    class ObjectPoolV2{
    private:
        struct Slot{
            alignas(T) std::byte storage[sizeof(T)];
            uint32_t generation = 0;

            T* get(){ return std::launder(reinterpret_cast<T*>(&storage)); }
            const T* get() const{ return std::launder(reinterpret_cast<const T*>(&storage)); }

            Slot() = default;
        };
        std::vector<Slot> slots;
        using Indexes = std::vector<Index>;
        Indexes freeIndexes;

    public:
        ObjectPoolV2() = default;
        ~ObjectPoolV2(){
            std::sort(freeIndexes.begin(), freeIndexes.end());

            Index freeIdxPtr = 0;
            for(Index i=0; i<slots.size(); ++i){
                if(freeIdxPtr<freeIndexes.size() && freeIndexes[freeIdxPtr]==i){
                    ++freeIdxPtr;
                    continue;
                }
                std::destroy_at(slots[i].get());
            }
        }

        HandleV2 push(T&& t){
            Index freeIndex = std::numeric_limits<uint32_t>::max();

            if(freeIndexes.size() > 0){
                freeIndex = freeIndexes[freeIndexes.size() - 1];
                freeIndexes.resize(freeIndexes.size() - 1);
            }
            else{
                slots.resize(slots.size() + 1);
                freeIndex = slots.size() - 1;
            }
            std::construct_at(slots[freeIndex].get(), std::move(t));
            ++slots[freeIndex].generation;

            return {
                .index = freeIndex,
                .generation = slots[freeIndex].generation
            };
        }

        template<typename... Args>
        HandleV2 emplace(Args... args){
            return push(T(std::forward<Args>(args)...));
        }

        void remove(HandleV2 handle){
            if(slots[handle.index].generation != handle.generation)
                throw std::out_of_range(std::format(
                    "Handle(Index={}) generation {} is mismatched. (valid generation={})",
                    handle.index, handle.generation, slots[handle.index].generation
                ));
            std::destroy_at(slots[handle.index].get());
            freeIndexes.push_back(handle.index);
        }

        void clear(){
            std::sort(freeIndexes.begin(), freeIndexes.end());

            Index freeIdxPtr = 0;
            for(Index i=0; i<slots.size(); ++i){
                if(freeIdxPtr<freeIndexes.size() && freeIndexes[freeIdxPtr]==i){
                    ++freeIdxPtr;
                    continue;
                }
                std::destroy_at(slots[i].get());
            }

            freeIndexes.clear();
            for(Index i=0; i<slots.size(); ++i)
                freeIndexes.push_back(i);

            slots.clear();
        }

        T& operator[](HandleV2 handle){
            if(slots[handle.index].generation != handle.generation)
                throw std::out_of_range(std::format(
                    "Handle(Index={}) generation {} is mismatched. (valid generation={})",
                    handle.index, handle.generation, slots[handle.index].generation
                ));
            return *slots[handle.index].get();
        }
        const T& operator[](HandleV2 handle) const{
                throw std::out_of_range(std::format(
                    "Handle(Index={}) generation {} is mismatched. (valid generation={})",
                    handle.index, handle.generation, slots[handle.index].generation
                ));
            return *slots[handle.index].get();
        }

        void reserve(size_t size){
            if(size <= slots.size())
                return;
            for(auto i=slots.size(); i<size; ++i)
                freeIndexes.push_back(i);
            slots.resize(size);
        }

        size_t size() const{
            return slots.size() - freeIndexes.size();
        }
        size_t capacity() const{
            return slots.size();
        }
    };
} // namespace ModernBoy

#endif // MODERNBOY_OBJECT_POOL_HPP