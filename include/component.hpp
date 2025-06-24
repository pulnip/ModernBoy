#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include <utility>
#include "common/type.hpp"
#include "fwd.hpp"
#include "raw_resource.hpp"
#include "input/state.hpp"

#define COMPONENT_ALIGN (8)

namespace ModernBoy
{
    struct alignas(COMPONENT_ALIGN) ResourceComponent{
        EntityID actor = UINT32_MAX;

        Handle handle;
    };
    struct alignas(COMPONENT_ALIGN) MeshComponent{
        EntityID actor = UINT32_MAX;

        Handle accessHandle;
        // TextureHandle textureHandle;
    };
    template<typename T>
    struct alignas(COMPONENT_ALIGN) ValueComponent{
        EntityID actor = UINT32_MAX;
        T value;
    };

    using TransformComponent = ValueComponent<Transform>;
    using CameraComponent = ValueComponent<Camera>;
    using InputComponent = ValueComponent<Input::InputMap>;

        enum class ComponentType{
        INVALID   = -1,
        TRANSFORM = 0,
        CAMERA    = 1,
        MESH      = 2,
        INPUT     = 3,
        NUM_COMPONENT = 4,
    };
    constexpr ArchetypeBit TRANSFORM_BIT = (1 << ArchetypeBit(ComponentType::TRANSFORM));
    constexpr ArchetypeBit CAMERA_BIT    = (1 << ArchetypeBit(ComponentType::CAMERA));
    constexpr ArchetypeBit MESH_BIT      = (1 << ArchetypeBit(ComponentType::MESH));
    constexpr ArchetypeBit INPUT_BIT     = (1 << ArchetypeBit(ComponentType::INPUT));

    constexpr auto RENDER_BIT = TRANSFORM_BIT | MESH_BIT;
    constexpr auto VIEW_BIT   = TRANSFORM_BIT | CAMERA_BIT;
    constexpr auto KB_IN_BIT = TRANSFORM_BIT | INPUT_BIT;

    template<typename T>
    consteval ArchetypeBit bit_of(){
        if constexpr(std::same_as<T, TransformComponent>)
            return TRANSFORM_BIT;
        else if constexpr(std::same_as<T, CameraComponent>)
            return CAMERA_BIT;
        else if constexpr(std::same_as<T, MeshComponent>)
            return MESH_BIT;
        else if constexpr(std::same_as<T, InputComponent>)
            return INPUT_BIT;
        else if constexpr(std::same_as<T, RenderTask>)
            return RENDER_BIT;
        else if constexpr(std::same_as<T, ViewTask>)
            return VIEW_BIT;
        else if constexpr(std::same_as<T, InputTask>)
            return KB_IN_BIT;
        else
            return static_cast<ArchetypeBit>(0);
    }

    bool subset(ArchetypeBit a, ArchetypeBit b);
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP