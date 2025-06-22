#include <toml++/toml.h>
#include "asset_loader.hpp"
#include "game/app_state.hpp"

using namespace ModernBoy;

AssetLoader::AssetLoader(AppState& app):app(app){}

static Transform parseTransform(const toml::v3::table& table);
static Camera parseCamera(const toml::v3::table& table);


void AssetLoader::loadActors(const std::string& fileName){
    toml::table tbl = toml::parse_file(fileName);

    auto actors = *tbl["entities"].as_array();
    for(const auto& actor_node: actors){
        const auto& actor = *actor_node.as_table();
        uint32_t id = issueID();

        const auto& trans_tbl = *actor["transform"].as_table();
        auto transform = parseTransform(trans_tbl);
        auto transformHandle = app.transformManager.create(std::move(transform));

        auto model_arr = *actor["model"].as_array();
        for(const auto& parts_node: model_arr){
            const auto& parts = *parts_node.as_table();

            const auto meshFile = *parts["mesh"].value<std::string>();
            auto meshHandles = app.meshLoader.load(meshFile);

            // const auto& texFile = *parts["diffuse"].value<std::string>();

            for(size_t i=0; i<meshHandles.size(); ++i)
                app.renderSystem.create(id, transformHandle, meshHandles[i]);
        }
        auto scriptModule = *actor["script"]["file"].value<std::string>();
        app.controller.loadScriptModule(scriptModule);

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
        app.inputSystem.create(id, behaviours, transformHandle);
    }
}
void AssetLoader::loadCamera(const std::string& fileName){
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
        auto transformHandle = app.transformManager.create(std::move(transform));

        auto c = cam["camera"].as_table();
        auto camera = parseCamera(*c);
        auto cameraHandle = app.cameraManager.create(std::move(camera));

        app.viewSystem.create(id, enabled, transformHandle, cameraHandle);
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
