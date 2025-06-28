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
    struct Slot{
        Resource data;
        uint32_t generation = 0;
        uint32_t refCount = 0;
    };

    template<typename Resource>
    class ResourceManager{
    private:
        using ResourceSlot = Slot<Resource>;

        AppState& app;

        ObjectPool<ResourceSlot> pool;
        // check if Named Resource is already Loaded.
        std::unordered_map<std::string, Index> pathToIndex;
        std::unordered_map<Index, std::string> indexToPath;

        static ResourceHandle makeHandle(Index index, uint32_t generation);
        // ToDo. public for just shader.
    public:
        [[nodiscard]] ResourceHandle emplace(Resource&& x){
            Index index = pool.newIndex();

            ResourceSlot& slot = pool[index];
            slot.data = std::move(x);
            ++slot.generation;
            slot.refCount = 1;

            return makeHandle(index, slot.generation);
        }
    private:
        [[nodiscard]] ResourceHandle link(const std::string& fileName){
            Index index = pathToIndex.at(fileName);
            ResourceSlot& slot = pool.get(index);
            ++slot.refCount;

            return makeHandle(index, slot.generation);
        }

    public:
        ResourceManager(AppState& app)
        :app(app){}

        bool isExist(const std::string& fileName){
            auto it = pathToIndex.find(fileName);
            return it != pathToIndex.end();
        }
        [[nodiscard]] ResourceHandle load(const std::string& fileName){
            if(isExist(fileName))
                return link(fileName);

            auto resource = import<Resource>(app, fileName);
            auto handle = emplace(std::move(resource));

            pathToIndex.emplace(std::make_pair(fileName, handle.index));
            indexToPath.emplace(std::make_pair(handle.index, fileName));

            return handle;
        }
        void unload(ResourceHandle handle){
            [[unlikely]] if(!isValid(handle)) return;

            ResourceSlot& slot = pool[handle.index];
            --slot.refCount;

            if(slot.refCount == 0){
                pool.erase(handle.index);
            }
        }

        bool isValid(ResourceHandle handle) const{
            // check if handle itself is valid
            [[unlikely]] if(!isValid(handle)) return false;

            // check if relation between handle and slot is valid
            const ResourceSlot& slot = pool[handle.index];
            return (slot.refCount > 0) && (slot.generation == handle.generation);
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