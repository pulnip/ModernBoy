#include "resource_manager.hpp"
#include "fwd.hpp"
#include "script/type.hpp"

using namespace ModernBoy;

template<> MeshHandle
ResourceManager<Mesh>::makeHandle(Index index){
    return MeshHandle{ResourceType::MESH, index};
}
template<> TextureHandle
ResourceManager<Texture>::makeHandle(Index index){
    return TextureHandle{ResourceType::TEXTURE, index};
}
template<> ShaderHandle
ResourceManager<Shader>::makeHandle(Index index){
    return ShaderHandle{ResourceType::SHADER, index};
}
template<> ModuleHandle
ResourceManager<Module>::makeHandle(Index index){
    return ModuleHandle{ResourceType::MODULE, index};
}

template<> ResourceHandle
ResourceManager<Script::Module>::getHandle(
    const std::string& name) const{
    auto slotIndex = nameToIndex.at(name);

    return ResourceHandle{
        .type = ResourceType::MODULE,
        .index = slotIndex,
    };
}
