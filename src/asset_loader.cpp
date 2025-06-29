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
static MeshComponent parseMeshComponent(
    const std::string& meshFile, MeshManager& meshManager,
    const std::string& textureFile, TextureManager& textureManager,
    const std::string& shaderFile, ShaderManager& shaderManager,
    NativePtr layerPtr, UI& gui
);
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

        if(actor.contains("model")){
            auto model = actor["model"];
            auto mc = parseMeshComponent(
                model["mesh"].value_or("cube"), app.meshManager,
                model["texture"].value_or("metal_logo.png"), app.textureManager,
                model["shader"].value_or("default"), app.shaderManager,
                app.renderSystem.context.metalLayer, app.ui
            );

            bit = bit | MESH_BIT;
            chunk.mesh = mc;
        }
        auto ic = parseInputComponent(actor["input"].as_table(), app);

        if(tc.has_value()){
            bit = bit | TRANSFORM_BIT;
            chunk.transform = tc.value();
        }
        if(cc.has_value()){
            bit = bit | CAMERA_BIT;
            chunk.camera = cc.value();
        }
        if(ic.has_value()){
            bit = bit | INPUT_BIT;
            chunk.input = ic.value();
        }

        auto actor_id = app.createActor(bit, std::move(chunk));
        std::println("Actor {}: {}, {}", actor_id, name, bit);
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

static MeshComponent parseMeshComponent(
    const std::string& meshFile, MeshManager& meshManager,
    const std::string& textureFile, TextureManager& textureManager,
    const std::string& shaderFile, ShaderManager& shaderManager,
    NativePtr metalLayer, UI& gui
){
    auto meshHandle = meshManager.emplace(meshFile, metalLayer);
    auto textureHandle = textureManager.emplace(textureFile, metalLayer);
    auto shaderHandle = shaderManager.emplace(
        shaderFile.compare("default") != 0 ?
        shaderFile : "asset/shader/ModernBoy.metallib",
        metalLayer, &gui
    );

    return dangled<MeshComponent>(meshHandle,
        textureHandle, shaderHandle);
}

static std::tuple<std::string, std::string, std::string>
parseInputTrigger(const std::string& text);
static std::tuple<std::string, std::string>
parseInputAction(const std::string& text);

static std::optional<InputComponent> parseInputComponent(
    const toml::table* table, AppState& app
){
    if(table==nullptr)
        return std::nullopt;

    auto component = dangled<InputComponent>();

    uint8_t i=0;
    for(const auto& n: *(*table)["map"].as_array()){
        const auto& input = *n.as_table();

        auto triggerText = input["trigger"].value<std::string>().value();
        auto [d, keyText, stateText] = parseInputTrigger(triggerText);

        auto button = Input::toButton(keyText);
        auto state = Input::toButtonState(stateText);

        auto actionText = input["action"].value<std::string>().value();
        auto [mod, func] = parseInputAction(actionText);

        auto moduleHandle = app.getHandle<Script::Module>(mod);
        auto func_id = app.scriptInvoker.registerFunction(func);

        component.triggers[i] = {
            .button = button,
            .onState = state
        };
        component.actions[i] = {
            .moduleHandle = moduleHandle,
            .function = func_id
        };
        component.numAction = ++i;
    }

    return component;
}

static std::tuple<std::string, std::string, std::string>
parseInputTrigger(const std::string& text){
    std::regex re(R"(\.)");
    std::sregex_token_iterator iter(text.begin(), text.end(), re, -1);
    std::sregex_token_iterator end;

    std::array<std::string, 3> result;
    for(size_t i=0; i<3; ++i){
        result[i] = (iter++)->str();
    }

    return {result[0], result[1], result[2]};
}
static std::tuple<std::string, std::string>
parseInputAction(const std::string& text){
    std::regex re(R"(\.)");
    std::sregex_token_iterator iter(text.begin(), text.end(), re, -1);
    std::sregex_token_iterator end;

    std::array<std::string, 2> result;
    for(size_t i=0; i<2; ++i){
        result[i] = (iter++)->str();
    }

    return {result[0], result[1]};
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

        [[maybe_unused]] auto mod = app.moduleManager.emplace(
            moduleName.value(), fileNames,
            app.scriptInvoker.engine);
    }
}