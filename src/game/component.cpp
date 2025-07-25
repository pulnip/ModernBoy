#include <limits>
#include <unordered_map>
#include "util/bit.hpp"
#include "common/helper.hpp"
#include "game/component.hpp"

using namespace ModernBoy::Game;

size_t ModernBoy::Game::size_of(ArchetypeBit bit){
    size_t size = sizeof(EntityID);
    if(bit & TRANSFORM_BIT)
        size += sizeof(TransformComponent);
    if(bit & CAMERA_BIT)
        size += sizeof(CameraComponent);
    if(bit & COLOR_BIT)
        size += sizeof(ColorComponent);
    if(bit & MESH_BIT)
        size += sizeof(MeshComponent);
    if(bit & ACTION_BIT)
        size += sizeof(ActionComponent);
    if(bit & SCRIPT_BIT)
        size += sizeof(ScriptComponent);
    if(bit & INPUT_BIT)
        size += sizeof(InputComponent);
    if(bit & RIGIDBODY_BIT)
        size += sizeof(RigidbodyComponent);
    if(bit & ELEMENT_BIT)
        size += sizeof(ElementComponent);
    if(bit & SPHERECOLLIDER_BIT)
        size += sizeof(SphereColliderComponent);

    if(bit & COLLIDED_BIT)
        size += sizeof(Collided);
    return size;
}

bool ModernBoy::Game::subset(ArchetypeBit a, ArchetypeBit b){
    return (a & b) == a;
}

template<> TransformComponent
ModernBoy::Game::dangled(Vec3 position, Vec4 rotation, Vec3 scale){
    return {std::numeric_limits<EntityID>::max(),
        true, position, rotation, scale };
}
template<> TransformComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(),
        false, zeros(), unitQuat(), ones() };
}
template<> CameraComponent
ModernBoy::Game::dangled(Camera camera, bool active){
    return {std::numeric_limits<EntityID>::max(),
        active, camera};
}
template<> CameraComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(),
        false, {}};
}
template<> ColorComponent
ModernBoy::Game::dangled(Vec4 color){
    return {.actor = std::numeric_limits<EntityID>::max(),
        .color = color};
}
template<> MeshComponent
ModernBoy::Game::dangled(MeshHandle meshHandle,
    TextureHandle textureHandle, ShaderHandle shaderHandle
){
    return {std::numeric_limits<EntityID>::max(),
        true, meshHandle, 1.0f, textureHandle, shaderHandle};
}
template<> MeshComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(), false,
        invalidResourceHandle(),
        1.0f,
        invalidResourceHandle(),
        invalidResourceHandle()};
}
template<> ActionComponent
ModernBoy::Game::dangled(ModuleHandle handle, FunctionID id){
    return {std::numeric_limits<EntityID>::max(),
        true, handle, id
    };
}
template<> ScriptComponent
ModernBoy::Game::dangled(ObjectHandle handle){
    return {std::numeric_limits<EntityID>::max(),
        true, handle
    };
}
template<> InputComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(),
        true, 0, {}, {},
        invalidResourceHandle()
    };
}

template<> RigidbodyComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(),
        zeros(), false, 1
    };
}

template<> SphereColliderComponent
ModernBoy::Game::dangled(Vec3 position, float radius){
    return {std::numeric_limits<EntityID>::max(),
        position, radius
    };
}
