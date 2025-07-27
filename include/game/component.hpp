#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include <type_traits>
#include "common/type.hpp"
#include "fwd.hpp"
#include "task.hpp"
#include "game/game_fwd.hpp"
#include "script/type.hpp"

namespace ModernBoy::Game
{
    #define DEFINE_COMPONENT(name, ...) \
    struct name{ \
        EntityID entity; \
        bool isActive; \
        __VA_ARGS__ \
    };

    DEFINE_COMPONENT(Transform,
        DEFINE_TRANSFORM;
    );
    struct Camera{
        EntityID entity;

        bool isActive;
        CameraType type;
        float fov;
        float nearPlane;
        float farPlane;
        Projection projection;
    };
    struct Color{
        EntityID entity;

        Vec4 color;
    };
    struct Model{
        EntityID entity;

        bool isActive;
        ResourceHandle handle;
        float alpha;
        TextureHandle textureHandle;
        ShaderHandle shaderHandle;
    };
    struct Action{
        EntityID entity;

        bool isActive;
        ModuleHandle moduleHandle;
        FunctionID updateFunc;
    };
    struct ScriptObject{
        EntityID entity;

        bool isActive;
        ObjectHandle handle;
    };
    struct Input{
        EntityID entity;

        bool isActive;
        bool isMoveEnabled;
        bool isJumpEnabled;
        bool isSkillEnabled;
        ModuleHandle handle;
    };
    struct LifeSpan{
        EntityID entity;

        bool isAlive;
    };
    struct Rigidbody{
        EntityID entity;

        Vec3 velocity;
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
    struct Element{
        EntityID entity;

        ElementType type;
    };
    struct PhysicsMaterial{
        float bounciness;
        float friction;
    };
    struct SphereCollider{
        EntityID entity;

        Vec3 position;
        float radius;
    };
    struct FixedBoxCollider{
        EntityID entity;

        Vec3 position;
        Vec3 scale;
        PhysicsMaterial material;
    };
    struct BoxCollider{
        EntityID entity;

        PhysicsMaterial material;
    }; static_assert(std::is_trivially_copyable_v<BoxCollider>);

    // Event Tags
    struct Collided{ EntityID entity; }; static_assert(std::is_trivially_copyable_v<Collided>);

    // Entity-to-Entity Event 
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
        X(       COLLISION) \
        X(        COLLIDED)
    #define ARCHETYPE_PAIRS \
        X(       Transform,        TRANSFORM) \
        X(          Camera,           CAMERA) \
        X(           Color,            COLOR) \
        X(           Model,             MESH) \
        X(          Action,           ACTION) \
        X(    ScriptObject,           SCRIPT) \
        X(           Input,            INPUT) \
        X(        LifeSpan,         LIFESPAN) \
        X(       Rigidbody,        RIGIDBODY) \
        X(         Element,          ELEMENT) \
        X(  SphereCollider,   SPHERECOLLIDER) \
        X(FixedBoxCollider, FIXEDBOXCOLLIDER) \
        X(     BoxCollider,      BOXCOLLIDER) \
        X(        PhysicalCollision,        COLLISION) \
        X(                 Collided,         COLLIDED)
    #define COMPOSIT_PAIRS \
        X(                 DrawTask,             DRAW) \
        X(                 ViewTask,             VIEW) \
        X(               ActionTask,           ACTION)

    #define ASSERT_TRIVIAL(type, name) static_assert(std::is_trivially_copyable_v<type>);
    #define X ASSERT_TRIVIAL
    ARCHETYPE_PAIRS
    #undef X

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

    template<typename T>
    constexpr size_t offset_of(ArchetypeBit bit){
        #define COMP_OFFSET(type, name) \
            if(std::same_as<T, type>) \
                return offset; \
            if(bit & bit_of<type>()) \
                offset += sizeof(type);
        size_t offset = sizeof(EntityID);
        #define X COMP_OFFSET
        ARCHETYPE_PAIRS
        #undef X
        return offset;
    }
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP