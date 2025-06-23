#include <toml++/toml.h>
#include "asset_loader.hpp"
#include "app_state.hpp"
#include "resource_handle.hpp"
#include "component.hpp"

using namespace ModernBoy;

AssetLoader::AssetLoader(AppState& app):app(app){}

static Transform parseTransform(const toml::v3::table& table);
static Camera parseCamera(const toml::v3::table& table);

struct DataLinker{
    std::optional<Transform> transform = std::nullopt;
    std::optional<Camera> camera = std::nullopt;
    std::vector<MeshHandle> meshHandles{};
    std::optional<Input::InputMap> input = std::nullopt;
};
static void linkActor(AppState& app, EntityID actor,
    const DataLinker& linker);

void AssetLoader::loadActors(const std::string& fileName){
    toml::table tbl = toml::parse_file(fileName);

    auto actors = *tbl["entities"].as_array();
    for(const auto& actor_node: actors){
        const auto& actor = *actor_node.as_table();
        // new Actor
        ComponentSlot components;
        DataLinker datalinker;

        // transform component
        if(auto trans_tbl = actor["transform"].as_table()){
            auto transform = parseTransform(*trans_tbl);
            datalinker.transform = transform;
            auto poolIndex = app.transformPool.emplace(
                TransformComponent(transform)
            );
            components.emplace(std::make_pair(
                ComponentType::TRANSFORM,
                SlotIndexes(poolIndex))
            );
        }

        // mesh & texture & shader component
        if(auto model_arr = actor["model"].as_array()){
            for(const auto& parts_node: *model_arr){
                const auto& parts = *parts_node.as_table();

                // mesh component
                const auto meshFile = *parts["mesh"].value<std::string>();
                auto meshHandles = app.meshLoader.load(meshFile);
                datalinker.meshHandles = meshHandles;

                SlotIndexes indexes(meshHandles.size());
                for(size_t i=0; i<meshHandles.size(); ++i){
                    indexes[i] = app.meshPool.emplace(
                        MeshComponent(meshHandles[i])
                    );
                }
                components.emplace(std::make_pair(
                    ComponentType::MESH, indexes
                ));

                // ToDo. texture component
                // const auto& texFile = *parts["diffuse"].value<std::string>();
            }
        }
        auto scriptModule = *actor["script"]["file"].value<std::string>();
        app.controller.loadScriptModule(scriptModule);

        // input component
        if(auto input_arr = actor["input"].as_array()){
            Input::InputMap inputMap;
            for(const auto& input_node: *input_arr){
                const auto& input = *input_node.as_table();
    
                auto button_text = *input["key"].value<std::string>();
                auto state_text = *input["trigger"].value<std::string>();
                auto behaviour = *input["behaviour"].value<std::string>();

                auto button = Input::convert(button_text);
                auto state = Input::toButtonState(state_text);

                Input::addInput(inputMap, button, state, behaviour);
            }
            datalinker.input = inputMap;
            auto poolIndex = app.inputPool.emplace(
                InputComponent(inputMap)
            );
            components.emplace(std::make_pair(
                ComponentType::INPUT, poolIndex
            ));
        }
        EntityID id = app.actorTable.emplace(std::move(components));
        linkActor(app, id, datalinker);
    }
}
void AssetLoader::loadCamera(const std::string& fileName){
    toml::table tbl = toml::parse_file(fileName);

    auto cameras = tbl["entities"].as_array();

    for(const auto& cam_node: *cameras){
        const auto& cam = *cam_node.as_table();
        // new Actor
        ComponentSlot components;
        DataLinker datalinker;

        auto name = *cam["name"].value<std::string>();
        auto type = *cam["type"].value<std::string>();
        bool enabled = type.compare("MainCamera")==0;
        std::println("{} {}", name, type);

        if(auto t = cam["transform"].as_table()){
            auto transform = parseTransform(*t);
            datalinker.transform = transform;
            auto poolIndex = app.transformPool.emplace(
                TransformComponent(transform)
            );
            components.emplace(std::make_pair(
                ComponentType::TRANSFORM, poolIndex
            ));
        }
        else{
            // warning!
        }
        if(auto c = cam["camera"].as_table()){
            auto camera = parseCamera(*c);
            datalinker.camera = camera;
            auto poolIndex = app.cameraPool.push(
                CameraComponent(camera)
            );
            components.emplace(std::make_pair(
                ComponentType::CAMERA, poolIndex
            ));
        }
        else{
            // warning!
        }
        EntityID id = app.actorTable.push(components);
        linkActor(app, id, datalinker);
        // TODO: Multi View
        break;
    }
}

static Transform parseTransform(
    const toml::v3::table& table
){
    Transform transform{};
    auto p = table["position"].as_array();
    auto r = table["rotation"].as_array();
    auto s = table["scale"].as_array();

    for(size_t i=0; i<3; ++i)
        transform.pos[i] = *((*p)[i]).value<double>();
    for(size_t i=0; i<4; ++i)
        transform.rot[i] = *((*r)[i]).value<double>();
    for(size_t i=0; i<3; ++i)
        transform.scl[i] = *((*s)[i]).value<double>();
    return transform;
}

static Camera parseCamera(const toml::v3::table& table){
    auto fov = *table["fov"].value<double>();
    auto near = *table["nearPlane"].value<double>();
    auto far = *table["farPlane"].value<double>();
    auto proj = *table["projection"].value<std::string>();

    Projection projection = Projection::PERSPECTIVE;
    if(proj.compare("orthographic") == 0)
        projection = Projection::ORTHOGRAPHIC;

    return Camera{
        .fov = static_cast<float>(fov),
        .nearPlane = static_cast<float>(near),
        .farPlane = static_cast<float>(far),
        .projection = projection
    };
}

static void linkActor(AppState& app, EntityID actor,
    const DataLinker& linker
){
    bool hasTransform = linker.transform.has_value();
    bool hasMesh = linker.meshHandles.size() > 0;
    bool hasCamera = linker.camera.has_value();
    bool hasInput = linker.input.has_value();

    if(hasTransform && hasMesh){
        auto transform = linker.transform.value();
        std::vector<RenderTask> tasks(linker.meshHandles.size());
        for(size_t i=0; i<tasks.size(); ++i)
            tasks[i] = RenderTask{transform, linker.meshHandles[i]};
        app.renderSystem.emplace(actor, tasks);
    }

    if(hasTransform && hasCamera){
        auto transform = linker.transform.value();
        std::vector<ViewTask> tasks(1);
        tasks[0] = ViewTask{transform, linker.camera.value()};
        app.viewSystem.emplace(actor, tasks);
    }

    if(hasInput && hasTransform){
        auto transform = linker.transform.value();
        std::vector<InputTask> tasks;
        for(const auto& buttonMap: linker.input.value()){
            for(const auto& pair: buttonMap.second){
                tasks.emplace_back(InputTask{
                    buttonMap.first, pair.first, pair.second,
                    transform
                });
            }
        }
        app.inputSystem.emplace(actor, tasks);
    }
}
