#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include <array>
#include <utility>
#include "common/type.hpp"
#include "fwd.hpp"
#include "raw_resource.hpp"
#include "input/state.hpp"

#define COMPONENT_ALIGN (8)

namespace ModernBoy
{
    struct alignas(COMPONENT_ALIGN) TransformComponent{
        EntityID actor;

        bool isActive;
        Transform value;
    }; static_assert(std::is_pod_v<TransformComponent>);
    struct alignas(COMPONENT_ALIGN) CameraComponent{
        EntityID actor;

        bool isActive;
        Camera value;
    }; static_assert(std::is_pod_v<CameraComponent>);
    struct alignas(COMPONENT_ALIGN) MeshComponent{
        EntityID actor;

        bool isActive;
        ResourceHandle handle;
        TextureHandle textureHandle;
        ShaderHandle shaderHandle;
    }; static_assert(std::is_pod_v<MeshComponent>);
    constexpr auto MAX_KEYACTION_PAIR = 16;
    struct InputComponent{
        EntityID actor;

        bool isActive;
        uint8_t countMap;
        Input::KeyActionPair map[MAX_KEYACTION_PAIR];
    }; static_assert(std::is_trivially_copyable_v<InputComponent>);

    template<typename Component, typename ...T>
    Component dangled(T... args);

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