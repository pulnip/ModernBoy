#include "resource_manager.hpp"
#include "fwd.hpp"

using namespace ModernBoy;

template<>
ResourceHandle ResourceManager<Mesh>::makeResourceHandle(
    Index index, uint32_t generation
){
    return MeshHandle{ResourceType::MESH,
        index, generation};
}

template<>
ResourceHandle ResourceManager<Shader>::makeResourceHandle(
    Index index, uint32_t generation
){
    return ShaderHandle{ResourceType::SHADER,
        index, generation};
}
