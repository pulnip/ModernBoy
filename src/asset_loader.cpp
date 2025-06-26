#include <toml++/toml.h>
#include "common/type.hpp"
#include "common/helper.hpp"
#include "asset_loader.hpp"
#include "app_state.hpp"
#include "component.hpp"

using namespace ModernBoy;

AssetLoader::AssetLoader(AppState& app):app(app){}

static Transform parseTransform(const toml::v3::table& table);
static Camera parseCamera(const toml::v3::table& table);

void AssetLoader::loadActors(const std::string& fileName){
    toml::table tbl = toml::parse_file(fileName);

    auto actors = *tbl["entities"].as_array();
    for(const auto& actor_node: actors){
        const auto& actor = *actor_node.as_table();
        // new Actor
        ArchetypeBit bit = 0;
        SparseChunk chunk;

        std::string name = *actor["name"].value<std::string>();

        // transform component
        if(auto trans_tbl = actor["transform"].as_table()){
            auto transform = parseTransform(*trans_tbl);

            chunk.transform = dangled<TransformComponent>(transform);
            bit = bit | TRANSFORM_BIT;
        }

        // camera component
        if(auto cam_tbl = actor["camera"].as_table()){
            auto camera = parseCamera(*cam_tbl);
            bool enabled = camera.type==CameraType::MainCamera;

            chunk.camera = dangled<CameraComponent>(camera, enabled);
            bit = bit | CAMERA_BIT;
        }

        // mesh component
        if(auto model_tbl = actor["model"].as_table()){
            const auto meshFile = (*model_tbl)["mesh"]
                .value<std::string>().value_or("Sphere");
            auto accessHandle = app.meshManager.load(meshFile);

            chunk.mesh = MeshComponent{
                INVALID_ENTITY, true, accessHandle};
            bit = bit | MESH_BIT;
            // ToDo. texture component
            // const auto& texFile = *parts["diffuse"].value<std::string>();
            // ToDo. How to handle multiple model?
        }

        if(auto script_tbl = actor["script"].as_table()){
            auto scriptFile = (*script_tbl)["file"].value<std::string>();
            if(scriptFile.has_value()){
                app.controller.loadScriptModule(scriptFile.value());
            }
        }

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
            chunk.input = dangled<InputComponent,
                Input::InputMap&&>(std::move(inputMap));
            bit = bit | INPUT_BIT;
        }
        [[maybe_unused]] auto actor_id = app.createActor(bit, std::move(chunk));
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
        transform.position.v[i] = *((*p)[i]).value<double>();
    for(size_t i=0; i<4; ++i)
        transform.rotation.v[i] = *((*r)[i]).value<double>();
    for(size_t i=0; i<3; ++i)
        transform.scale.v[i] = *((*s)[i]).value<double>();
    return transform;
}

static Camera parseCamera(const toml::v3::table& table){
    auto typeText = *table["type"].value<std::string>();
    auto fov = *table["fov"].value<double>();
    auto near = *table["nearPlane"].value<double>();
    auto far = *table["farPlane"].value<double>();
    auto projText = *table["projection"].value<std::string>();

    auto type = cameraType(typeText);

    Projection projection = Projection::PERSPECTIVE;
    if(projText.compare("orthographic") == 0)
        projection = Projection::ORTHOGRAPHIC;

    return Camera{
        .type = type,
        .fov = static_cast<float>(fov),
        .nearPlane = static_cast<float>(near),
        .farPlane = static_cast<float>(far),
        .projection = projection
    };
}
