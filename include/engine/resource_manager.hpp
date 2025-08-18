#ifndef MODERNBOY_RESOURCE_MANAGER_HPP
#define MODERNBOY_RESOURCE_MANAGER_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "core/math/type.hpp"
#include "engine/fwd.hpp"
#include "core/memory/object_pool.hpp"
#include "engine/log/log.hpp"

namespace ModernBoy
{
    // ensure uniqueness of Resource
    template<typename Resource, typename Key>
    class ResourceManager{
    private:
        // storage class
        ObjectPoolV2<Resource> pool;

        std::unordered_map<Key, Handle> uuidToHandle;
        std::unordered_map<Handle, Key, HandleHash> handleToUUID;

    public:
        ResourceManager() = default;

        bool isExist(Key uuid){
            auto it = uuidToHandle.find(uuid);
            return it != uuidToHandle.end();
        }

        template<typename... Args>
        [[nodiscard]] Handle emplace(
            Key uuid, Args... args
        ){
            AppDebug("try to load: {}", uuid);
            if(isExist(uuid)){
                AppDebug("    already loaded.", uuid);
                return uuidToHandle.at(uuid);
            }

            auto handle = pool.emplace(std::forward<Args>(args)...);

            uuidToHandle.emplace(std::make_pair(uuid, handle));
            handleToUUID.emplace(std::make_pair(handle, uuid));

            AppDebug("    successfully loaded.", uuid);
            return handle;
        }
        void unload(Handle handle){
            pool.remove(handle);

            auto uuid = handleToUUID.at(handle);

            handleToUUID.erase(handle);
            uuidToHandle.erase(uuid);
        }

        auto& get(Handle handle){
            return pool[handle];
        }
        const auto& get(Handle handle) const{
            return pool[handle];
        }
        auto& get(Key id){
            return pool[uuidToHandle.at(id)];
        }
        const auto& get(Key id) const{
            return pool[uuidToHandle.at(id)];
        }

        size_t size() const{ return pool.size(); }
        size_t capacity() const{ return pool.capacity(); }
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_MANAGER_HPP