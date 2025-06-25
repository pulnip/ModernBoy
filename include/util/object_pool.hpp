#ifndef MODERNBOY_OBJECT_POOL_HPP
#define MODERNBOY_OBJECT_POOL_HPP

#include <cassert>
#include <ranges>
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
            pool.resize(count);
            freeIndexes.reserve(count - size());
            for(size_t i=size(); i<count; ++i){
                freeIndexes.emplace(i);
            }
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
            size_t size = ranges.size();
            std::vector<Index> indexes(size);
            size_t i=0;

            if(freeIndexes.size() <= size){
                resize(pool.size()+size);
                for(Index freeIndex: freeIndexes){
                    indexes[i++] = freeIndex;
                }
                i=0;
                freeIndexes.clear();
                for(auto&& x: ranges){
                    pool[indexes[i++]] = std::forward<decltype(x)>(x);
                }
            }
            else{
                for(auto&& x: ranges){
                    indexes[i++] = emplace(std::forward<decltype(x)>(x));
                }
            }

            return newIndex;
        }

        Index push(const T& x) noexcept{
            Index new_index = issueSlot();
            pool[new_index] = x;

            return new_index;
        }
        void erase(Index index) noexcept{
            freeIndexes.insert(index);
        }

        T& get(Index index) noexcept{
            assert(index < pool.size());
            assert(freeIndexes.find(index) == freeIndexes.end());
            return pool[index];
        }
        const T& get(Index index) const noexcept{
            assert(index < pool.size());
            assert(freeIndexes.find(index) == freeIndexes.end());
            return pool[index];
        }
        T& operator[](Index index) noexcept{
            assert(index < pool.size());
            assert(freeIndexes.find(index) == freeIndexes.end());
            return pool[index];
        }
        const T& operator[](Index index) const noexcept{
            assert(index < pool.size());
            assert(freeIndexes.find(index) == freeIndexes.end());
            return pool[index];
        }
        Ts get() const{ return pool; }

        size_t size() const{ return pool.size() - freeIndexes.size(); }
        size_t capacity() const{ return pool.size(); }
    };
} // namespace ModernBoy

#endif // MODERNBOY_OBJECT_POOL_HPP