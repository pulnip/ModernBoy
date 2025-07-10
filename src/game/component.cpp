#include <limits>
#include <unordered_map>
#include "util/bit.hpp"
#include "common/helper.hpp"
#include "game/component.hpp"

using namespace ModernBoy::Game;

void ModernBoy::Game::setChunk(void* dst,
    const SparseChunk& chunk, ArchetypeBit bit
){
    if(bit & TRANSFORM_BIT)
        dst = Util::chunkcpy(dst, chunk.transform);
    if(bit & CAMERA_BIT)
        dst = Util::chunkcpy(dst, chunk.camera);
    if(bit & MESH_BIT)
        dst = Util::chunkcpy(dst, chunk.mesh);
    if(bit & ACTION_BIT)
        dst = Util::chunkcpy(dst, chunk.action);
    if(bit & INPUT_BIT)
        dst = Util::chunkcpy(dst, chunk.input);
}


size_t ModernBoy::Game::size_of(ArchetypeBit bit){
    size_t size = 0;
    if(bit & TRANSFORM_BIT)
        size += sizeof(TransformComponent);
    if(bit & CAMERA_BIT)
        size += sizeof(CameraComponent);
    if(bit & MESH_BIT)
        size += sizeof(MeshComponent);
    if(bit & ACTION_BIT)
        size += sizeof(ActionComponent);
    if(bit & INPUT_BIT)
        size += sizeof(InputComponent);
    return size;
}

bool ModernBoy::Game::subset(ArchetypeBit a, ArchetypeBit b){
    return (a & b) == a;
}

template<> TransformComponent
ModernBoy::Game::dangled(Transform transform){
    return {std::numeric_limits<EntityID>::max(),
        true, transform };
}
template<> TransformComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(),
        false, identity() };
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
template<> MeshComponent
ModernBoy::Game::dangled(MeshHandle meshHandle,
    TextureHandle textureHandle, ShaderHandle shaderHandle
){
    return {std::numeric_limits<EntityID>::max(),
        true, meshHandle, textureHandle, shaderHandle};
}
template<> MeshComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(), false,
        invalidResourceHandle(),
        invalidResourceHandle(),
        invalidResourceHandle()};
}
template<> ActionComponent
ModernBoy::Game::dangled(ModuleHandle handle, FunctionID id){
    return {std::numeric_limits<EntityID>::max(),
        true, handle, id
    };
}
template<> InputComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(),
        true, 0, {}, {},
        invalidResourceHandle()
    };
}

