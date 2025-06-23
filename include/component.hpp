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
    constexpr ArchetypeBit TRANSFORM_BIT = (1 << ArchetypeBit(ComponentType::TRANSFORM));
    constexpr ArchetypeBit CAMERA_BIT = (1 << ArchetypeBit(ComponentType::CAMERA));
    constexpr ArchetypeBit MESH_BIT = (1 << ArchetypeBit(ComponentType::MESH));
    constexpr ArchetypeBit INPUT_BIT = (1 << ArchetypeBit(ComponentType::INPUT));

    struct ResourceComponent{
        EntityID actor = UINT32_MAX;

        ResourceHandle handle;
    };
    struct MeshComponent{
        EntityID actor = UINT32_MAX;

        std::vector<MeshHandle> meshHandles;
        // TextureHandle textureHandle;
    };
    template<typename T>
    struct ValueComponent{
        EntityID actor = UINT32_MAX;
        T value;
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