#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include <array>
#include <utility>
#include "common/type.hpp"
#include "fwd.hpp"
#include "raw_resource.hpp"
#include "input/state.hpp"
#include "script/type.hpp"

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
    struct alignas(COMPONENT_ALIGN) InputComponent{
        EntityID actor;

        bool isActive;
        uint8_t numAction;
        Input::Trigger triggers[16];
        Script::Action actions[16];
    }; static_assert(std::is_pod_v<InputComponent>);
    struct alignas(COMPONENT_ALIGN) LifeSpanComponent{
        bool isAlive;
    };
    struct alignas(COMPONENT_ALIGN) PhysicsComponent{
        bool useGravity;
        float mass;
    };
    enum class ElementType{
        FIRE,
        EARTH,
        DENDRO,
        WIND,
        WATER,
        ICE,
        ELECTRIC
    };
    struct alignas(COMPONENT_ALIGN) ElementComponent{

    };

    template<typename Component, typename ...T>
    Component dangled(T... args);

        enum class ComponentType{
        INVALID   = -1,
        TRANSFORM = 0,
        CAMERA    = 1,
        MESH      = 2,
        INPUT     = 3,
        LIFESPAN  = 4,
        PHYSICS   = 5,
        ELEMENT   = 6,
        NUM_COMPONENT = 7,
    };
    constexpr ArchetypeBit TRANSFORM_BIT = (1 << ArchetypeBit(ComponentType::TRANSFORM));
    constexpr ArchetypeBit CAMERA_BIT    = (1 << ArchetypeBit(ComponentType::CAMERA));
    constexpr ArchetypeBit MESH_BIT      = (1 << ArchetypeBit(ComponentType::MESH));
    constexpr ArchetypeBit INPUT_BIT     = (1 << ArchetypeBit(ComponentType::INPUT));
    constexpr ArchetypeBit LIFESPAN_BIT  = (1 << ArchetypeBit(ComponentType::LIFESPAN));
    constexpr ArchetypeBit PHYSICS_BIT   = (1 << ArchetypeBit(ComponentType::PHYSICS));
    constexpr ArchetypeBit ELEMENT_BIT   = (1 << ArchetypeBit(ComponentType::ELEMENT));

    constexpr auto RENDER_BIT = TRANSFORM_BIT | MESH_BIT;
    constexpr auto VIEW_BIT   = TRANSFORM_BIT | CAMERA_BIT;
    constexpr auto KB_IN_BIT = INPUT_BIT;

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
        else if constexpr(std::same_as<T, LifeSpanComponent>)
            return LIFESPAN_BIT;
        else if constexpr(std::same_as<T, PhysicsComponent>)
            return PHYSICS_BIT;
        else if constexpr(std::same_as<T, ElementComponent>)
            return ELEMENT_BIT;
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
    template<typename Component>
    size_t offset_of(ArchetypeBit bit){
        size_t offset = 0;
        if(std::same_as<Component, TransformComponent>)
            return offset;
        if(bit & bit_of<TransformComponent>())
            offset += sizeof(TransformComponent);

        if(std::same_as<Component, CameraComponent>)
            return offset;
        if(bit & bit_of<CameraComponent>())
            offset += sizeof(CameraComponent);

        if(std::same_as<Component, MeshComponent>)
            return offset;
        if(bit & bit_of<MeshComponent>())
            offset += sizeof(MeshComponent);


        if(std::same_as<Component, InputComponent>)
            return offset;
        if(bit & bit_of<InputComponent>())
            offset += sizeof(InputComponent);

        if(std::same_as<Component, LifeSpanComponent>)
            return offset;
        if(bit & bit_of<LifeSpanComponent>())
            offset += sizeof(LifeSpanComponent);

        if(std::same_as<Component, PhysicsComponent>)
            return offset;
        if(bit & bit_of<PhysicsComponent>())
            offset += sizeof(PhysicsComponent);

        if(std::same_as<Component, PhysicsComponent>)
            return offset;
        if(bit & bit_of<PhysicsComponent>())
            offset += sizeof(PhysicsComponent);
        return offset;
    }
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP