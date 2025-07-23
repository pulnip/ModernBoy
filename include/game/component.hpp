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
        DEFINE_TRANSFORM;
    }; static_assert(std::is_trivially_copyable_v<TransformComponent>);
    struct alignas(COMPONENT_ALIGN) CameraComponent{
        EntityID actor;

        bool isActive;
        Camera value;
    }; static_assert(std::is_trivially_copyable_v<CameraComponent>);
    struct alignas(COMPONENT_ALIGN) ColorComponent{
        EntityID actor;

        Vec4 color;
    }; static_assert(std::is_trivially_copyable_v<ColorComponent>);
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
    struct alignas(COMPONENT_ALIGN) ScriptComponent{
        EntityID actor;

        bool isActive;
        ObjectHandle handle;
    }; static_assert(std::is_trivially_copyable_v<ScriptComponent>);
    struct alignas(COMPONENT_ALIGN) InputComponent{
        EntityID actor;

        bool isActive;
        bool isMoveEnabled;
        bool isJumpEnabled;
        bool isSkillEnabled;
        ModuleHandle handle;
    }; static_assert(std::is_trivially_copyable_v<InputComponent>); 
    struct alignas(COMPONENT_ALIGN) LifeSpanComponent{
        EntityID actor;

        bool isAlive;
    }; static_assert(std::is_trivially_copyable_v<LifeSpanComponent>);
    struct alignas(COMPONENT_ALIGN) RigidbodyComponent{
        EntityID actor;

        Vec3 velocity;
        bool useGravity;
        float mass;
    }; static_assert(std::is_trivially_copyable_v<RigidbodyComponent>);
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
    }; static_assert(std::is_trivially_copyable_v<ElementComponent>);
    struct PhysicsMaterial{
        float bounciness;
        float friction;
    };
    struct SphereColliderComponent{
        EntityID actor;

        Vec3 position;
        float radius;
        PhysicsMaterial material;
    }; static_assert(std::is_trivially_copyable_v<SphereColliderComponent>);
    struct FixedBoxColliderComponent{
        EntityID actor;

        Vec3 position;
        Vec3 scale;
        PhysicsMaterial material;
    }; static_assert(std::is_trivially_copyable_v<FixedBoxColliderComponent>);
    struct BoxColliderComponent{
        EntityID actor;

        PhysicsMaterial material;
    }; static_assert(std::is_trivially_copyable_v<BoxColliderComponent>);

    struct SparseChunk{
        TransformComponent transform;
        CameraComponent camera;
        MeshComponent mesh;
        ActionComponent action;
        InputComponent input;
        RigidbodyComponent rigidbody;
    };

    // Entity-to-Entity Event Component
    struct PhysicalCollision{
        Vec3 force;
    }; static_assert(std::is_trivially_copyable_v<PhysicalCollision>);

    #define ARCHETYPES \
        X(       TRANSFORM) \
        X(          CAMERA) \
        X(           COLOR) \
        X(            MESH) \
        X(          ACTION) \
        X(          SCRIPT) \
        X(           INPUT) \
        X(        LIFESPAN) \
        X(       RIGIDBODY) \
        X(         ELEMENT) \
        X(  SPHERECOLLIDER) \
        X(FIXEDBOXCOLLIDER) \
        X(     BOXCOLLIDER) \
        X(       COLLISION)
    #define ARCHETYPE_PAIRS \
        X(       TransformComponent,        TRANSFORM) \
        X(          CameraComponent,           CAMERA) \
        X(           ColorComponent,            COLOR) \
        X(            MeshComponent,             MESH) \
        X(          ActionComponent,           ACTION) \
        X(          ScriptComponent,           SCRIPT) \
        X(           InputComponent,            INPUT) \
        X(        LifeSpanComponent,         LIFESPAN) \
        X(       RigidbodyComponent,        RIGIDBODY) \
        X(         ElementComponent,          ELEMENT) \
        X(  SphereColliderComponent,   SPHERECOLLIDER) \
        X(FixedBoxColliderComponent, FIXEDBOXCOLLIDER) \
        X(     BoxColliderComponent,      BOXCOLLIDER) \
        X(        PhysicalCollision,        COLLISION)
    #define COMPOSIT_PAIRS \
        X(                 DrawTask,             DRAW) \
        X(                 ViewTask,             VIEW) \
        X(               ActionTask,           ACTION)

    enum{
        #define NAME_INDEX(name) name##_INDEX,
        #define X NAME_INDEX
        ARCHETYPES
        #undef X
        NUM_ARCHETYPES
    };

    #define DECL_BIT(name) constexpr ArchetypeBit \
        name##_BIT = (1 << name##_INDEX);
    #define X DECL_BIT
    ARCHETYPES
    #undef X
    constexpr auto DRAW_BIT    = TRANSFORM_BIT |      MESH_BIT;
    constexpr auto VIEW_BIT    = TRANSFORM_BIT |    CAMERA_BIT;
    constexpr auto PHYSICS_BIT = TRANSFORM_BIT | RIGIDBODY_BIT;

    size_t size_of(ArchetypeBit bit);

    template<typename T>
    consteval ArchetypeBit bit_of();
    template<typename... Ts>
    consteval ArchetypeBit bits_of(){
        return (... | bit_of<Ts>());
    }
    #define TYPE_TO_BIT(type, name) template<> \
        consteval ArchetypeBit bit_of<type>(){ return name##_BIT; }
    #define X TYPE_TO_BIT
    ARCHETYPE_PAIRS
    COMPOSIT_PAIRS
    #undef X

    bool subset(ArchetypeBit a, ArchetypeBit b);

    template<typename Component>
    size_t offset_of(ArchetypeBit bit){
        #define COMP_OFFSET(type, name) \
            if(std::same_as<Component, type>) \
                return offset; \
            if(bit & bit_of<type>()) \
                offset += sizeof(type);
        size_t offset = 0;
        #define X COMP_OFFSET
        ARCHETYPE_PAIRS
        #undef X
        return offset;
    }

    void setChunk(void* dst, const SparseChunk& chunk,
        ArchetypeBit bit);

    template<typename Component, typename ...T>
    Component dangled(T... args);
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP