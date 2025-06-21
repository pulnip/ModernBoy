#ifndef MODERNBOY_RESOURCE_MANAGER_HPP
#define MODERNBOY_RESOURCE_MANAGER_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "resource_handle.hpp"

namespace ModernBoy{
    template<typename T>
    struct ResourceSlot{
        T data;
        uint32_t generation = 0;
        bool alive = false;
    };

    template<typename T>
    class ResourceManager{
    public:
        using Handle = ResourceHandle<T>;
        using Slot = ResourceSlot<T>;

    private:
        std::vector<Slot> slots;
        std::vector<uint32_t> freeSlots;
        // check if Named Resource is already Loaded.
        std::unordered_map<std::string, Handle> pathToHandle;

    public:
        [[nodiscard]] Handle create(T&& in_data){
            [[unlikely]] if(!freeSlots.empty()){
                uint32_t index = freeSlots.back();
                freeSlots.pop_back();

                Slot& slot = slots[index];
                slot.data = std::move(in_data);
                ++slot.generation;
                slot.alive = true;

                return Handle{index, slot.generation};
            }

            uint32_t index = slots.size();

            slots.push_back(Slot{std::move(in_data), 1, true});
            return Handle{index, 1};
        }
        [[nodiscard]] Handle load(const std::string& in_fileName,
            std::function<T(const std::string&)> in_loader
        ){
            // check if Named Resource is already Loaded.
            if(auto it = pathToHandle.find(in_fileName); it != pathToHandle.end()){
                return it->second;
            }

            T data = in_loader(in_fileName);

            Handle handle = create(data);
            pathToHandle[in_fileName] = handle;

            return handle;
        }

        bool isValid(Handle in_handle) const{
            // check if handle itself is valid
            [[unlikely]] if(!in_handle.isValid()) return false;
            [[unlikely]] if(in_handle.index >= slots.size()) return false;

            // check if relation between handle and slot is valid
            const Slot& slot = slots[in_handle.index];
            return slot.alive && (slot.generation == in_handle.generation);
        }

        void unload(Handle in_handle){
            [[unlikely]] if(!isValid(in_handle)) return;

            Slot& slot = slots[in_handle.index];
            slot.alive = false;
            // invalidate previous handle
            ++slot.generation;

            freeSlots.push_back(in_handle.index);
        }

        T* get(Handle in_handle){
            [[unlikely]] if(!isValid(in_handle)) return nullptr;
            return &slots[in_handle.index].data;
        }
        const T* get(Handle in_handle) const{
            [[unlikely]] if(!isValid(in_handle)) return nullptr;
            return &slots[in_handle.index].data;
        }

        size_t size() const{ return slots.size() - freeSlots.size(); }
        size_t capacity() const{ return slots.size(); }
    };

    using TransformManager = ResourceManager<Transform>;
    using CameraManager = ResourceManager<Camera>;
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_MANAGER_HPP