#include <optional>
#include <regex>
#include <toml++/toml.h>
#include "common/type.hpp"
#include "common/helper.hpp"
#include "asset_loader.hpp"
#include "app_state.hpp"
#include "component.hpp"
#include "script/invoker.hpp"
#include "script/type.hpp"

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
            chunk.mesh = mc.value();
        }
        if(ic.has_value()){
            bit = bit | INPUT_BIT;
            chunk.input = ic.value();
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
        .projection = proj
    };

    return dangled<CameraComponent>(
        camera, type==CameraType::MainCamera);
}

static std::optional<MeshComponent> parseMeshComponent(
    const toml::table* table, MeshManager& meshManager
){
    if(table==nullptr)
        return std::nullopt;

    const auto meshFile = (*table)["mesh"]
        .value<std::string>().value_or("Sphere");
    auto handle = meshManager.load(meshFile);

    // ToDo. texture component
    // const auto& texFile = *parts["diffuse"].value<std::string>();
    // ToDo. How to handle multiple model?

    return dangled<MeshComponent>(handle);
}

static std::tuple<std::string, std::string, std::string>
parseInputTrigger(const std::string& text);

static std::optional<InputComponent> parseInputComponent(
    const toml::table* table, AppState& app
){
    if(table==nullptr)
        return std::nullopt;

    auto component = dangled<InputComponent>();

    auto scriptFile = (*table)["file"].value<std::string>().value();

    for(const auto& n: *(*table)["map"].as_array()){
        const auto& input = *n.as_table();

        auto triggerText = input["trigger"].value<std::string>().value();
        auto [d, keyText, stateText] = parseInputTrigger(triggerText);

        auto key = Input::toButton(keyText);
        auto state = Input::toButtonState(stateText);
        auto trigger = (key << 2) | state;

        auto actionText = input["action"].value<std::string>().value();
        // auto a_id = app.controller.loadModuleFunction(actionText);

        // component.map[component.countMap] = {trigger, a_id};
        // ++component.countMap;
        // if(component.countMap == MAX_KEYACTION_PAIR)
        //     break;
    }

    return component;
}

static std::tuple<std::string, std::string, std::string>
parseInputTrigger(const std::string& text){
    std::regex re(R"(\.)");
    std::sregex_token_iterator iter(text.begin(), text.end(), re, -1);
    std::sregex_token_iterator end;

    std::vector<std::string> result(3);
    for(; iter!=end; ++iter){
        result.push_back(iter->str());
    }

    return {result[0], result[1], result[2]};
}

void AssetLoader::loadScripts(const std::string& fileName){
    toml::table tbl = toml::parse_file(fileName);

    auto modules = *tbl["module"].as_array();
    for(const auto& m: modules){
        const auto& module_ = m.as_table();

        const auto moduleName = (*module_)["name"].value<std::string>();

        std::vector<std::string> fileNames;
        std::vector<std::string> funcNames;

        const auto files = *(*module_)["file"].as_array();
        for(const auto& f: files){
            const auto& file = f.as_table();

            const auto& fileName = (*file)["name"].value<std::string>();
            fileNames.push_back(fileName.value());
        }
        const auto funcs = *(*module_)["function"].as_array();
        for(const auto& f: funcs){
            const auto& func = f.as_table();
            const auto funcName = (*func)["name"].value<std::string>();
            funcNames.push_back(funcName.value());
        }
        app.scriptInvoker.buildModule(moduleName.value(),
            fileNames, funcNames);
        break;
    }

}