#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include <type_traits>
#include "common/type.hpp"
#include "fwd.hpp"
#include "task.hpp"
#include "game/game_fwd.hpp"
#include "script/type.hpp"

#define COMPONENT_ALIGN (8)

namespace ModernBoy::Game
{
    struct alignas(COMPONENT_ALIGN) TransformComponent{
        EntityID actor;

        bool isActive;
        Transform value;
    }; static_assert(std::is_trivially_copyable_v<TransformComponent>);
    struct alignas(COMPONENT_ALIGN) CameraComponent{
        EntityID actor;

        bool isActive;
        Camera value;
    }; static_assert(std::is_trivially_copyable_v<CameraComponent>);
    struct alignas(COMPONENT_ALIGN) MeshComponent{
        EntityID actor;

        bool isActive;
        ResourceHandle handle;
        TextureHandle textureHandle;
        ShaderHandle shaderHandle;
    }; static_assert(std::is_trivially_copyable_v<MeshComponent>);
    struct alignas(COMPONENT_ALIGN) ActionComponent{
        EntityID actor;

        bool isActive;
        ModuleHandle moduleHandle;
        FunctionID updateFunc;
    }; static_assert(std::is_trivially_copyable_v<ActionComponent>); 
    struct alignas(COMPONENT_ALIGN) InputComponent{
        EntityID actor;

        bool isActive;
        bool isMoveEnabled;
        bool isJumpEnabled;
        bool isSkillEnabled;
        ModuleHandle handle;
    };
    struct alignas(COMPONENT_ALIGN) LifeSpanComponent{
        EntityID actor;

        bool isAlive;
    }; static_assert(std::is_trivially_copyable_v<LifeSpanComponent>);
    struct alignas(COMPONENT_ALIGN) PhysicsComponent{
        EntityID actor;

        bool useGravity;
        float mass;
    }; static_assert(std::is_trivially_copyable_v<PhysicsComponent>);
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
    };static_assert(std::is_trivially_copyable_v<ElementComponent>);

    struct SparseChunk{
        TransformComponent transform;
        CameraComponent camera;
        MeshComponent mesh;
        ActionComponent action;
        InputComponent input;
    };

    template<typename Component, typename ...T>
    Component dangled(T... args);

        enum class ComponentType{
        INVALID   = -1,
        TRANSFORM = 0,
        CAMERA    = 1,
        MESH      = 2,
        ACTION    = 3,
        INPUT     = 4,
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
    DECL_BIT(LIFESPAN);
    DECL_BIT(PHYSICS);
    DECL_BIT(ELEMENT);
    DECL_BIT(ACTION);

    constexpr auto DRAW_BIT   = TRANSFORM_BIT | MESH_BIT;
    constexpr auto VIEW_BIT   = TRANSFORM_BIT | CAMERA_BIT;

    template<typename T>
    consteval ArchetypeBit bit_of(){
        if constexpr(std::same_as<T, TransformComponent>)
            return TRANSFORM_BIT;
        else if constexpr(std::same_as<T, CameraComponent>)
            return CAMERA_BIT;
        else if constexpr(std::same_as<T, MeshComponent>)
            return MESH_BIT;
        else if constexpr(std::same_as<T, ActionComponent>)
            return ACTION_BIT;
        else if constexpr(std::same_as<T, InputComponent>)
            return INPUT_BIT;
        else if constexpr(std::same_as<T, LifeSpanComponent>)
            return LIFESPAN_BIT;
        else if constexpr(std::same_as<T, PhysicsComponent>)
            return PHYSICS_BIT;
        else if constexpr(std::same_as<T, ElementComponent>)
            return ELEMENT_BIT;
        else if constexpr(std::same_as<T, DrawTask>)
            return DRAW_BIT;
        else if constexpr(std::same_as<T, ViewTask>)
            return VIEW_BIT;
        else if constexpr(std::same_as<T, ActionTask>)
            return ACTION_BIT;
        else
            return static_cast<ArchetypeBit>(0);
    }
    template<typename... Ts>
    consteval ArchetypeBit bits_of(){
        return (... | bit_of<Ts>());
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
        COMP_OFFSET(Action)
        COMP_OFFSET(Input)
        COMP_OFFSET(LifeSpan)
        COMP_OFFSET(Physics)
        COMP_OFFSET(Element)
        return offset;
    }
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP