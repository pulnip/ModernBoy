#include "resource_manager.hpp"
#include "fwd.hpp"
#include "script/module.hpp"

using namespace ModernBoy;

template<> ResourceHandle
ResourceManager<Mesh>::makeHandle(
    Index index, uint32_t generation
){
    return MeshHandle{ResourceType::MESH,
        index, generation};
}
template<> ResourceHandle
ResourceManager<Texture>::makeHandle(
    Index index, uint32_t generation
){
    return TextureHandle{ResourceType::TEXTURE,
        index, generation};
}
template<> ResourceHandle
ResourceManager<Shader>::makeHandle(
    Index index, uint32_t generation
){
    return ShaderHandle{ResourceType::SHADER,
        index, generation};
}
template<> ResourceHandle
ResourceManager<Script::Module>::makeHandle(
    Index index, uint32_t generation
){
    return ShaderHandle{ResourceType::MODULE,
        index, generation};
}

template<> ResourceHandle
ResourceManager<Script::Module>::getHandle(
    const std::string& name) const{
    auto slotIndex = nameToIndex.at(name);

    return ResourceHandle{
        .type = ResourceType::MODULE,
        .index = slotIndex,
        .generation = pool[slotIndex].generation
    };
}
