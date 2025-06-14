#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include <toml++/toml.h>
#include "resource_manager.hpp"

namespace ModernBoy{
    template<typename MeshLoader,
        typename RenderTaskManager>
    class AssetLoader{
        uint32_t seed = 0;
        MeshLoader& meshLoader;
        TransformManager& transformManager;
        RenderTaskManager& renderTaskManager;

        uint32_t issueID(){ return seed++; }

    public:
        AssetLoader(TransformManager& transformManager, MeshLoader& meshLoader,
            RenderTaskManager& renderTaskManager)
        :meshLoader(meshLoader), transformManager(transformManager),
        renderTaskManager(renderTaskManager){}

        void loadActor(const std::string& fileName){
            uint32_t id = issueID();
            toml::table tbl = toml::parse_file(fileName);

            std::string name = *tbl["name"].value<std::string>();
            auto transformHandle = transformManager.create(Transform{});

            std::string meshFile = *tbl["mesh"]["file"].value<std::string>();
            auto meshHandles = meshLoader.load(meshFile);


            for(size_t i=0; i<meshHandles.size(); ++i)
                renderTaskManager.create(id, transformHandle, meshHandles[i]);
        }
    };
}

#endif // MODERNBOY_ASSET_LOADER_HPP