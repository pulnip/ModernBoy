#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include <toml++/toml.h>

namespace ModernBoy{
    template<typename MeshLoader, typename MeshComponentSystem>
    class AssetLoader{
        uint32_t seed = 0;
        MeshLoader& meshLoader;
        MeshComponentSystem& meshComponentSystem;

        uint32_t issueID(){ return seed++; }

    public:
        AssetLoader(MeshLoader& meshLoader, MeshComponentSystem& meshComponentSystem)
        :meshLoader(meshLoader), meshComponentSystem(meshComponentSystem){}

        void loadActor(const std::string& fileName){
            uint32_t id = issueID();
            toml::table tbl = toml::parse_file(fileName);

            std::string name = *tbl["name"].value<std::string>();

            std::string meshFile = *tbl["mesh"]["file"].value<std::string>();
            auto meshHandles = meshLoader.load(meshFile);
            for(size_t i=0; i<meshHandles.size(); ++i)
                meshComponentSystem.create(id, meshHandles[i]);
        }
    };
}

#endif // MODERNBOY_ASSET_LOADER_HPP