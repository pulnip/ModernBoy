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
    struct alignas(COMPONENT_ALIGN) ScriptComponent{
        EntityID actor;

        bool isActive;
        ModuleHandle handle;
    };
    struct alignas(COMPONENT_ALIGN) LifeSpanComponent{
        EntityID actor;

        bool isAlive;
    }; static_assert(std::is_pod_v<LifeSpanComponent>);
    struct alignas(COMPONENT_ALIGN) PhysicsComponent{
        EntityID actor;

        bool useGravity;
        float mass;
    }; static_assert(std::is_pod_v<PhysicsComponent>);
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
        EntityID actor;

        ElementType type;
    };static_assert(std::is_pod_v<ElementComponent>);

    template<typename Component, typename ...T>
    Component dangled(T... args);

        enum class ComponentType{
        INVALID   = -1,
        TRANSFORM = 0,
        CAMERA    = 1,
        MESH      = 2,
        INPUT     = 3,
        SCRIPT    = 4,
        LIFESPAN  = 5,
        PHYSICS   = 6,
        ELEMENT   = 7,
        NUM_COMPONENT = 8,
    };

    #define DECL_BIT(NAME) constexpr ArchetypeBit NAME##_BIT \
        = (1 << ArchetypeBit(ComponentType::NAME))

    DECL_BIT(TRANSFORM);
    DECL_BIT(CAMERA);
    DECL_BIT(MESH);
    DECL_BIT(INPUT);
    DECL_BIT(SCRIPT);
    DECL_BIT(LIFESPAN);
    DECL_BIT(PHYSICS);
    DECL_BIT(ELEMENT);

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
        else if constexpr(std::same_as<T, ScriptComponent>)
            return SCRIPT_BIT;
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
        #define COMP_OFFSET(NAME) \
            if(std::same_as<Component, NAME##Component>) \
                return offset; \
            if(bit & bit_of<NAME##Component>()) \
                offset += sizeof(NAME##Component);
        COMP_OFFSET(Transform)
        COMP_OFFSET(Camera)
        COMP_OFFSET(Mesh)
        COMP_OFFSET(Input)
        COMP_OFFSET(Script)
        COMP_OFFSET(LifeSpan)
        COMP_OFFSET(Physics)
        COMP_OFFSET(Element)
        return offset;
    }
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP