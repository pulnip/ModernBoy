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
