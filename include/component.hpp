#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include <utility>
#include "fwd.hpp"
#include "resource_handle.hpp"

namespace ModernBoy
{
    enum class ComponentType{
        INVALID = -1,
        TRANSFORM = 0,
        CAMERA = 1,
        MESH = 2,
        INPUT = 3,
        NUM_COMPONENT = 4,
    };

    struct ResourceComponent{
        EntityID actor = UINT32_MAX;

        ResourceHandle handle;

        ResourceComponent(ResourceHandle handle);
        ResourceComponent() = default;
        ~ResourceComponent() = default;
        ResourceComponent(const ResourceComponent& other) = default;
        ResourceComponent(ResourceComponent&&) = default;
        ResourceComponent& operator=(const ResourceComponent&) = default;
        ResourceComponent& operator=(ResourceComponent&&) = default;
    };
    struct MeshComponent{
        EntityID actor = UINT32_MAX;

        std::vector<MeshHandle> meshHandles;
        // TextureHandle textureHandle;

        MeshComponent(const std::vector<MeshHandle>& handles);
        MeshComponent() = default;
        ~MeshComponent() = default;
        MeshComponent(const MeshComponent& other) = default;
        MeshComponent(MeshComponent&&) = default;
        MeshComponent& operator=(const MeshComponent&) = default;
        MeshComponent& operator=(MeshComponent&&) = default;
    };
    template<typename T>
    struct ValueComponent{
        EntityID actor = UINT32_MAX;
        T value;

        ValueComponent(T value)
        :value(value){}
        ValueComponent() = default;
        ~ValueComponent() = default;
        ValueComponent(const ValueComponent& other){
            actor = other.actor;
            value = std::move(other.value);
        }
        ValueComponent(ValueComponent&&) = default;
        ValueComponent& operator=(const ValueComponent&) = default;
        ValueComponent& operator=(ValueComponent&&) = default;
    };
}
template<> struct std::hash<ModernBoy::ResourceComponent>{
    std::size_t operator()(
        const ModernBoy::ResourceComponent& key
    ) const noexcept{ return key.actor; }
};
template<typename T>
struct std::hash<ModernBoy::ValueComponent<T>>{
    std::size_t operator()(
        const ModernBoy::ValueComponent<T>& key
    ) const noexcept{ return key.actor; }
};


#endif // MODERNBOY_RESOURCE_COMPONENT_HPP