#include "resource_manager.hpp"
#include "fwd.hpp"

using namespace ModernBoy;

template<>
ResourceHandle ResourceManager<Mesh>::makeHandle(
    Index index, uint32_t generation
){
    return MeshHandle{ResourceType::MESH,
        index, generation};
}
template<>
ResourceHandle ResourceManager<Shader>::makeHandle(
    Index index, uint32_t generation
){
    return ShaderHandle{ResourceType::SHADER,
        index, generation};
}
template<>
ResourceHandle ResourceManager<Script::Module>::makeHandle(
    Index index, uint32_t generation
){
    return ShaderHandle{ResourceType::MODULE,
        index, generation};
}
