#include <limits>
#include <unordered_map>
#include "common/helper.hpp"
#include "game/component.hpp"

using namespace ModernBoy::Game;

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
template<> InputComponent
ModernBoy::Game::dangled(){
    return {std::numeric_limits<EntityID>::max(),
        true, 0, {}, {}};
}

