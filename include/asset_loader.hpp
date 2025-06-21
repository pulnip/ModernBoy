#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include <toml++/toml.h>
#include "resource_manager.hpp"
#include "task_system.hpp"
#include "view_system.hpp"
#include "input/state.hpp"
#include "input/component.hpp"
#include "input/controller.hpp"

#include <print>

namespace ModernBoy{
    Transform parseTransform(const toml::v3::table& table);
    Camera parseCamera(const toml::v3::table& table);

    template<typename MeshLoader,
        typename RenderSystem,
        typename InputSystem>
    class AssetLoader{
        uint32_t seed = 0;
        MeshLoader& meshLoader;
        TransformManager& transformManager;
        RenderSystem& renderSystem;
        CameraManager& cameraManager;
        ViewSystem& viewSystem;
        InputSystem& inputSystem;
        Input::Controller& controller;

        uint32_t issueID(){ return seed++; }

    public:
        AssetLoader(TransformManager& transformManager,
            MeshLoader& meshLoader,
            RenderSystem& renderSystem,
            CameraManager& cameraManager,
            ViewSystem& viewSystem,
            InputSystem& inputSystem,
            Input::Controller& controller
        ):meshLoader(meshLoader), transformManager(transformManager),
        renderSystem(renderSystem), cameraManager(cameraManager),
        viewSystem(viewSystem), inputSystem(inputSystem),
        controller(controller){}

        void loadActors(const std::string& fileName){
            toml::table tbl = toml::parse_file(fileName);

            auto actors = *tbl["entities"].as_array();
            for(const auto& actor_node: actors){
                const auto& actor = *actor_node.as_table();
                uint32_t id = issueID();

                const auto& trans_tbl = *actor["transform"].as_table();
                auto transform = parseTransform(trans_tbl);
                auto transformHandle = transformManager.create(std::move(transform));

                auto model_arr = *actor["model"].as_array();
                for(const auto& parts_node: model_arr){
                    const auto& parts = *parts_node.as_table();

                    const auto meshFile = *parts["mesh"].value<std::string>();
                    auto meshHandles = meshLoader.load(meshFile);

                    // const auto& texFile = *parts["diffuse"].value<std::string>();

                    for(size_t i=0; i<meshHandles.size(); ++i)
                        renderSystem.create(id, transformHandle, meshHandles[i]);
                }
                auto scriptModule = *actor["script"]["file"].value<std::string>();
                controller.loadScriptModule(scriptModule);

                auto input_arr = *actor["input"].as_array();
                Input::Behaviours behaviours;
                for(const auto& input_node: input_arr){
                    const auto& input = *input_node.as_table();

                    auto key_text = *input["key"].value<std::string>();
                    auto trigger_text = *input["trigger"].value<std::string>();
                    auto behaviour = *input["behaviour"].value<std::string>();

                    auto key = Input::convert(key_text);
                    auto trigger = Input::toButtonState(trigger_text);
                    behaviours.emplace_back(key, trigger, behaviour);
                }
                inputSystem.create(id, behaviours, transformHandle);
            }
        }

        void loadCamera(const std::string& fileName){
            toml::table tbl = toml::parse_file(fileName);

            auto cameras = tbl["entities"].as_array();

            for(const auto& cam_node: *cameras){
                uint32_t id = issueID();
                const auto& cam = *cam_node.as_table();

                auto name = *cam["name"].value<std::string>();
                auto type = *cam["type"].value<std::string>();
                bool enabled = type.compare("MainCamera")==0;
                std::println("{} {}", name, type);

                auto t = cam["transform"].as_table();
                auto transform = parseTransform(*t);
                auto transformHandle = transformManager.create(std::move(transform));

                auto c = cam["camera"].as_table();
                auto camera = parseCamera(*c);
                auto cameraHandle = cameraManager.create(std::move(camera));

                viewSystem.create(id, enabled, transformHandle, cameraHandle);
            }
        }
    };
}

#endif // MODERNBOY_ASSET_LOADER_HPP