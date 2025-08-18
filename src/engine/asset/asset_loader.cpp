#include <regex>
#include "engine/asset/asset_loader.hpp"
#include "engine/engine.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

Asset::AssetLoader::AssetLoader(Engine& engine)
:engine(engine){}

void Asset::AssetLoader::load(const SceneDescriptor& desc){
    load(desc.meshes);
}

void Asset::AssetLoader::load(const std::vector<MeshDescriptor>& meshes){
    for(const auto& meshDesc: meshes){
        if(table.contains(meshDesc.id.schemePath))
            continue;

        auto colon_pos = meshDesc.id.schemePath.find(':');
        auto scheme = meshDesc.id.schemePath.substr(0, colon_pos);
        auto path = meshDesc.id.schemePath.substr(colon_pos + 1);

        auto id = issueID();

        [[likely]] if(scheme == "file"){
            auto mesh = importMeshFile(path);

            auto handle = engine.meshManager.emplace(
                id, std::move(mesh));
        } else if(scheme == "embedded"){
            auto handle = engine.meshManager.emplace(
                id, path);
        }

        table.try_emplace(meshDesc.id.schemePath, id);
    }
}