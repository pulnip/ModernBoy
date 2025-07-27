#include <limits>
#include <unordered_map>
#include "util/bit.hpp"
#include "common/helper.hpp"
#include "game/component.hpp"

using namespace ModernBoy::Game;

size_t ModernBoy::Game::size_of(ArchetypeBit bit){
    size_t size = sizeof(EntityID);
    if(bit & TRANSFORM_BIT)
        size += sizeof(Transform);
    if(bit & CAMERA_BIT)
        size += sizeof(Camera);
    if(bit & COLOR_BIT)
        size += sizeof(Color);
    if(bit & MESH_BIT)
        size += sizeof(Model);
    if(bit & ACTION_BIT)
        size += sizeof(Action);
    if(bit & SCRIPT_BIT)
        size += sizeof(ScriptObject);
    if(bit & INPUT_BIT)
        size += sizeof(Input);
    if(bit & RIGIDBODY_BIT)
        size += sizeof(Rigidbody);
    if(bit & ELEMENT_BIT)
        size += sizeof(Element);
    if(bit & SPHERECOLLIDER_BIT)
        size += sizeof(SphereCollider);

    if(bit & COLLIDED_BIT)
        size += sizeof(Collided);
    return size;
}

bool ModernBoy::Game::subset(ArchetypeBit a, ArchetypeBit b){
    return (a & b) == a;
}
