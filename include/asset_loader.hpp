#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include <toml++/toml.h>
#include "resource_manager.hpp"
#include "task_manager.hpp"

#include <print>

namespace ModernBoy{
    Transform parseTransform(const toml::v3::table& table);
    Camera parseCamera(const toml::v3::table& table);

    template<typename MeshLoader,
        typename RenderTaskManager>
    class AssetLoader{
        uint32_t seed = 0;
        MeshLoader& meshLoader;
        TransformManager& transformManager;
        RenderTaskManager& renderTaskManager;
        CameraManager& cameraManager;
        ViewTaskManager& viewTaskManager;

        uint32_t issueID(){ return seed++; }

    public:
        AssetLoader(TransformManager& transformManager,
            MeshLoader& meshLoader,
            RenderTaskManager& renderTaskManager,
            CameraManager& cameraManager,
            ViewTaskManager& viewTaskManager
        ):meshLoader(meshLoader), transformManager(transformManager),
        renderTaskManager(renderTaskManager),
        cameraManager(cameraManager),viewTaskManager(viewTaskManager){}

        void loadActor(const std::string& fileName){
            uint32_t id = issueID();
            toml::table tbl = toml::parse_file(fileName);

            std::string name = *tbl["name"].value<std::string>();

            auto t=tbl["transform"].as_table();
            auto transform = parseTransform(*t);
            auto transformHandle = transformManager.create(std::move(transform));

            std::string meshFile = *tbl["mesh"]["file"].value<std::string>();
            auto meshHandles = meshLoader.load(meshFile);

            for(size_t i=0; i<meshHandles.size(); ++i)
                renderTaskManager.create(id, transformHandle, meshHandles[i]);
        }

        void loadCamera(const std::string& fileName){
            toml::table tbl = toml::parse_file(fileName);

            auto cameras = tbl["entities"].as_array();

            for(const auto& cam_node: *cameras){
                uint32_t id = issueID();
                const auto& cam = *cam_node.as_table();

                std::string name = *cam["name"].value<std::string>();
                std::string type = *cam["type"].value<std::string>();
                bool enabled = type.compare("MainCamera")==0;
                std::println("{} {}", name, type);

                auto t = cam["transform"].as_table();
                auto transform = parseTransform(*t);
                auto transformHandle = transformManager.create(std::move(transform));

                auto c = cam["camera"].as_table();
                auto camera = parseCamera(*c);
                auto cameraHandle = cameraManager.create(std::move(camera));

                viewTaskManager.create(id, enabled, transformHandle, cameraHandle);
            }
        }
    };
}

#endif // MODERNBOY_ASSET_LOADER_HPP