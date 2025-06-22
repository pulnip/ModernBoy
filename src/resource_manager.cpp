#include "resource_manager.hpp"
#include "fwd.hpp"

using namespace ModernBoy;

template<>
ResourceHandle ResourceManager<Transform>::makeHandle(
    uint32_t index, uint32_t generation
){
    return TransformHandle{ResourceType::TRANSFORM,
        index, generation};
}

template<>
ResourceHandle ResourceManager<Mesh>::makeHandle(
    uint32_t index, uint32_t generation
){
    return MeshHandle{ResourceType::MESH,
        index, generation};
}

template<>
ResourceHandle ResourceManager<Shader>::makeHandle(
    uint32_t index, uint32_t generation
){
    return ShaderHandle{ResourceType::SHADER,
        index, generation};
}

template<>
ResourceHandle ResourceManager<Camera>::makeHandle(
    uint32_t index, uint32_t generation
){
    return CameraHandle{ResourceType::CAMERA,
        index, generation};
}