#ifndef MODERNBOY_RESOURCE_MANAGER_HPP
#define MODERNBOY_RESOURCE_MANAGER_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "core/math/type.hpp"
#include "fwd.hpp"
#include "core/memory/object_pool.hpp"
#include "log.hpp"

namespace ModernBoy
{
    template<typename Resource>
    struct Slot{
        Resource data;
        uint32_t refCount = 0;
    };

    template<typename Resource>
    class ResourceManager{
    private:
        using ResourceSlot = Slot<Resource>;

        ObjectPool<ResourceSlot> pool;
        // check if Named Resource is already Loaded.
        std::unordered_map<std::string, Index> nameToIndex;
        std::unordered_map<Index, std::string> indexToName;

        static ResourceHandle makeHandle(Index index);

        [[nodiscard]] ResourceHandle load(Resource&& x){
            Index index = pool.newIndex();

            ResourceSlot& slot = pool[index];
            slot.data = std::move(x);
            slot.refCount = 1;

            return makeHandle(index);
        }
    public:
        ResourceManager(){}

        bool isExist(const std::string& name){
            auto it = nameToIndex.find(name);
            return it != nameToIndex.end();
        }

        [[nodiscard]] ResourceHandle link(const std::string& name){
            Index index = nameToIndex.at(name);
            ResourceSlot& slot = pool.get(index);
            ++slot.refCount;

            return makeHandle(index);
        }

        template<typename... Args>
        [[nodiscard]] ResourceHandle emplace(
            const std::string& name, Args... args
        ){
            AppDebug("try to load: {}", name);
            if(isExist(name)){
                AppDebug("    already loaded.", name);
                return link(name);
            }

            auto resource = Resource(std::forward<Args>(args)...);
            auto handle = load(std::move(resource));

            nameToIndex.emplace(std::make_pair(name, handle.index));
            indexToName.emplace(std::make_pair(handle.index, name));

            AppDebug("    successfully loaded.", name);
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
            return (slot.refCount > 0);
        }

        auto& get(ResourceHandle handle){
            return pool[handle.index].data;
        }
        const auto& get(ResourceHandle handle) const{
            return pool[handle.index].data;
        }
        auto& get(const std::string& name){
            return pool[nameToIndex.at(name)].data;
        }
        const auto& get(const std::string& name) const{
            return pool[nameToIndex.at(name)].data;
        }
        ResourceHandle getHandle(const std::string& name) const;

        size_t size() const{ return pool.size(); }
        size_t capacity() const{ return pool.capacity(); }
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_MANAGER_HPP