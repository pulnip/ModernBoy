#ifndef MODERNBOY_OBJECT_POOL_HPP
#define MODERNBOY_OBJECT_POOL_HPP

#include <cassert>
#include <ranges>
#include <print>
#include <vector>
#include <unordered_set>
#include "common/alias.hpp"

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
} // namespace ModernBoy

#endif // MODERNBOY_OBJECT_POOL_HPP