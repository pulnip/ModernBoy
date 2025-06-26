#include <toml++/toml.h>
#include "common/type.hpp"
#include "common/helper.hpp"
#include "asset_loader.hpp"
#include "app_state.hpp"
#include "component.hpp"

using namespace ModernBoy;

AssetLoader::AssetLoader(AppState& app):app(app){}

static std::optional<TransformComponent> parseTransformComponent(
    const toml::table* table);
static std::optional<CameraComponent> parseCameraComponent(
    const toml::table* table);
static std::optional<MeshComponent> parseMeshComponent(
    const toml::table* table, MeshManager& meshManager);
static std::optional<InputComponent> parseInputComponent(
    const toml::table* table, AppState& app);

void AssetLoader::loadActors(const std::string& fileName){
    toml::table tbl = toml::parse_file(fileName);

    auto entities = *tbl["entities"].as_array();
    for(const auto& entity: entities){
        const auto& actor = *entity.as_table();
        // new Actor
        std::string name = *actor["name"].value<std::string>();
        ArchetypeBit bit = 0;
        SparseChunk chunk;

        auto tc = parseTransformComponent(actor["transform"].as_table());
        auto cc = parseCameraComponent(actor["camera"].as_table());
        auto mc = parseMeshComponent(actor["model"].as_table(), app.meshManager);
        auto ic = parseInputComponent(actor["input"].as_table(), app);

        if(tc.has_value()){
            bit = bit | TRANSFORM_BIT;
            chunk.transform = tc.value();
        }
        if(cc.has_value()){
            bit = bit | CAMERA_BIT;
            chunk.camera = cc.value();
        }
        if(mc.has_value()){
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

static std::optional<TransformComponent> parseTransformComponent(
    const toml::table* table
){
    if(table==nullptr)
        return std::nullopt;
    auto p = *(*table)["position"].as_array();
    auto r = *(*table)["rotation"].as_array();
    auto s = *(*table)["scale"].as_array();

    Transform transform;
    for(size_t i=0; i<3; ++i)
        transform.position.v[i] = *p[i].value<double>();
    for(size_t i=0; i<4; ++i)
        transform.rotation.v[i] = *r[i].value<double>();
    for(size_t i=0; i<3; ++i)
        transform.scale.v[i] = *s[i].value<double>();
    return dangled<TransformComponent>(transform);
}

static std::optional<CameraComponent> parseCameraComponent(
    const toml::table* table
){
    if(table==nullptr)
        return std::nullopt;

    auto typeText = (*table)["type"].value<std::string>().value();
    auto type = cameraType(typeText);
    auto fov = (*table)["fov"].value<double>().value();
    auto near = (*table)["nearPlane"].value<double>().value();
    auto far = (*table)["farPlane"].value<double>().value();
    auto projText = (*table)["projection"].value<std::string>().value();
    auto proj = projection(projText);

    Camera camera{
        .type = type,
        .fov = static_cast<float>(fov),
        .nearPlane = static_cast<float>(near),
        .farPlane = static_cast<float>(far),
        .projection = projection
    };
}
