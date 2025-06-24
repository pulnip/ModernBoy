#ifndef MODERNBOY_RESOURCE_MANAGER_HPP
#define MODERNBOY_RESOURCE_MANAGER_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "common/type.hpp"
#include "fwd.hpp"
#include "util/object_pool.hpp"

namespace ModernBoy
{
    template<typename Resource>
    struct ResourceSlot{
        Resource data;
        uint32_t generation = 0;
        uint32_t refCount = 0;
    };

    template<typename Resource>
    class ResourceManager{
    private:
        using Slot = ResourceSlot<Resource>;
        using AccessHandle = Handle;
        using AccessSlot = ResourceSlot<std::vector<ResourceHandle>>;

        AppState& app;

        ObjectPool<Slot> pool;
        ObjectPool<AccessSlot> accessor;
        // check if Named Resource is already Loaded.
        std::unordered_map<std::string, Index> pathToIndex;
        std::unordered_map<Index, std::string> indexToPath;

        static ResourceHandle makeResourceHandle(Index index, uint32_t generation);
    public:
        [[nodiscard]] ResourceHandle emplace(Resource&& x){
            Index index = pool.newIndex();

            Slot& slot = pool[index];
            slot.data = std::move(x);
            ++slot.generation;
            slot.refCount = 1;

            return makeResourceHandle(index, slot.generation);
        }
    private:
        [[nodiscard]] AccessHandle link(const std::string& fileName){
            Index index = pathToIndex.at(fileName);
            AccessSlot& slot = accessor.get(index);
            ++slot.refCount;

            return AccessHandle{index, slot.generation};
        }
        bool isValid(ResourceHandle handle) const{
            // check if handle itself is valid
            [[unlikely]] if(!handle.isValid()) return false;

            // check if relation between handle and slot is valid
            const Slot& slot = pool[handle.index];
            return (slot.refCount > 0) && (slot.generation == handle.generation);
        }
        void unload(ResourceHandle handle){
            [[unlikely]] if(!isValid(handle)) return;

            Slot& slot = pool[handle.index];
            --slot.refCount;

            if(slot.refCount == 0){
                pool.erase(handle.index);
            }
        }

    public:
        ResourceManager(AppState& app)
        :app(app){}

        bool isExist(const std::string& fileName){
            auto it = pathToIndex.find(fileName);
            return it != pathToIndex.end();
        }
        [[nodiscard]] AccessHandle load(const std::string& fileName){
            if(isExist(fileName))
                return link(fileName);

            auto resources = ModernBoy::import<Resource>(app, fileName);
            Index accessIndex = accessor.emplace(AccessSlot{
                .data = std::vector<ResourceHandle>(resources.size()),
                .generation = 1,
                .refCount = 1
            });

            if(pool.capacity() < pool.size()+resources.size())
                pool.resize(pool.size()+resources.size());

            auto& handles = accessor.get(accessIndex).data;
            for(auto& resource: resources){
                Index poolIndex = pool.newIndex();
                pool.get(poolIndex).data = std::move(resource);
                handles.emplace_back(makeResourceHandle(poolIndex, 1));
            }

            pathToIndex.emplace(std::make_pair(fileName, accessIndex));
            indexToPath.emplace(std::make_pair(accessIndex, fileName));
            return AccessHandle{.index=accessIndex, .generation=1};
        }

        bool isValid(AccessHandle handle){
            [[unlikely]] if(!handle.isValid()) return false;

            const auto& slot = accessor.get(handle.index);
            return (slot.refCount > 0) && (slot.generation == handle.generation);
        }

        void unload(AccessHandle handle){
            if(isValid(handle)) return;

            AccessSlot& slot = accessor.get(handle.index);
            --slot.refCount;

            if(slot.refCount == 0){
                accessor.erase(handle.index);
                const auto& fileName = indexToPath.at(handle.index);
                pathToIndex.erase(fileName);
                indexToPath.erase(handle.index);
            }
        }

        auto& get(AccessHandle handle){
            return accessor[handle.index].data;
        }
        const auto& get(AccessHandle handle) const{
            return accessor[handle.index].data;
        }
        auto& get(ResourceHandle handle){
            return pool[handle.index].data;
        }
        const auto& get(ResourceHandle handle) const{
            return pool[handle.index].data;
        }


        size_t size() const{ return pool.size(); }
        size_t capacity() const{ return pool.capacity(); }
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_MANAGER_HPP