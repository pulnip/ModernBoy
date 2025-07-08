#include <optional>
#include <regex>
#if defined(USE_DIRECTX)
#elif defined(USE_METAL)
#include <SDL3/SDL_metal.h>
#elif defined(USE_OPENGL)
#endif
#include <toml++/toml.h>
#include "common/type.hpp"
#include "common/helper.hpp"
#include "log.hpp"
#include "asset_loader.hpp"
#include "app_state.hpp"
#include "game/component.hpp"
#include "script/invoker.hpp"
#include "script/type.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

AssetLoader::AssetLoader(AppState& app):app(app){
    loadAction("asset/action.toml");
    loadAsset("asset/actor.toml");
}

static std::tuple<ArchetypeBit, SparseChunk>
parseActor(const toml::table* ptr, AppState& app);
static std::tuple<std::string, std::vector<std::string>>
parseModule(const toml::table* ptr);

template<typename Component>
static std::optional<Component> parse(
    const toml::table*, AppState& app);

void AssetLoader::loadAsset(const std::string& fileName){
    AppDebug("Load Asset: {}", fileName);
    auto table = toml::parse_file(fileName);

    auto entities = *table["entities"].as_array();

    for(const auto& ntt: entities){
        const auto& entity = ntt.as_table();

        auto [bit, chunk] = parseActor(entity, app);

        app.world.create(bit, std::move(chunk));
    }
}

void AssetLoader::loadAction(const std::string& fileName){
    AppDebug("Load Action: {}", fileName);
    auto tbl = toml::parse_file(fileName);

    auto modules = *tbl["module"].as_array();
    for(const auto& md: modules){
        const auto module_ = md.as_table();

        auto [mdName, funcs] = parseModule(module_);

        [[maybe_unused]] auto mdHandle = app.append<Script::Module>(mdName, funcs);
    }
}


template<>
std::optional<TransformComponent>
parse<TransformComponent>(
    const toml::table* ptr, AppState&
){
    if(ptr == nullptr)
        return std::nullopt;
    const auto& table = *ptr;

    auto p = *table["position"].as_array();
    auto r = *table["rotation"].as_array();
    auto s = *table["scale"].as_array();

    Transform transform;
    for(size_t i=0; i<3; ++i)
        transform.position.v[i] = *p[i].value<double>();
    for(size_t i=0; i<4; ++i)
        transform.rotation.v[i] = *r[i].value<double>();
    for(size_t i=0; i<3; ++i)
        transform.scale.v[i] = *s[i].value<double>();
    return dangled<TransformComponent>(transform);
}

template<>
std::optional<CameraComponent>
parse<CameraComponent>(
    const toml::table* ptr, AppState&
){
    if(ptr==nullptr)
        return std::nullopt;
    const auto& table = *ptr;

    auto typeText = table["type"].value<std::string>().value();
    auto type = cameraType(typeText);
    auto fov = table["fov"].value<double>().value();
    auto near = table["nearPlane"].value<double>().value();
    auto far = table["farPlane"].value<double>().value();
    auto projText = table["projection"].value<std::string>().value();
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
template<>
std::optional<MeshComponent>
parse<MeshComponent>(
    const toml::table* ptr, AppState& app
){
    if(ptr == nullptr)
         return std::nullopt;
    const auto& model = *ptr;

    std::string meshFile = model["mesh"].value_or("cube");
    auto meshHandle = app.append<Mesh>(meshFile);

    std::string textureFile = model["texture"].value_or("metal_logo.png");
    auto textureHandle = app.append<Texture>(textureFile);

    std::string shaderFile = model["shader"].value_or("default");
    auto shaderHandle = app.append<Shader>(
        shaderFile.compare("default") != 0 ?
            shaderFile : "asset/shader/ModernBoy.metallib");

    return dangled<MeshComponent>(meshHandle,
        textureHandle, shaderHandle);
}

static std::tuple<std::string, std::string, std::string>
parseInputTrigger(const std::string& text);
static std::tuple<std::string, std::string>
parseInputAction(const std::string& text);
template<>
std::optional<InputComponent>
parse<InputComponent>(
    const toml::table* ptr, AppState& app
){
    if(ptr==nullptr)
        return std::nullopt;
    const auto& script = *ptr;

    auto component = dangled<InputComponent>();

    auto moduleName = *script["module"].value<std::string>();
    auto moduleHandle = app.query<Script::Module>(moduleName);

    component.handle = moduleHandle;

    return component;
}

static std::tuple<ArchetypeBit, SparseChunk>
parseActor(const toml::table* ptr, AppState& app){
    if(ptr == nullptr)
        return {};
    const auto& actor = *ptr;
    // new Actor
    std::string name = *actor["name"].value<std::string>();
    ArchetypeBit bit = 0;
    SparseChunk chunk;

    auto tc = parse<TransformComponent>(
        actor["transform"].as_table(), app);
    auto cc = parse<CameraComponent>(
        actor["camera"].as_table(), app);

    std::optional<MeshComponent> mc;
    if(actor.contains("model")){
        auto model = actor["model"].as_table();

        mc = parse<MeshComponent>(model, app);
    }

    auto ic = parse<InputComponent>(
        actor["script"].as_table(), app);

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

    GameDebug("Actor loaded, name: {}, archetype: {}", name, bit);
    return {bit, chunk};
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

static std::tuple<std::string, std::vector<std::string>>
parseModule(const toml::table* ptr){
    if(ptr==nullptr)
        return {};
    const auto& module_ = *ptr->as_table();

    const auto moduleName = module_["name"].value<std::string>();

    std::vector<std::string> fileNames;
    std::vector<std::string> funcNames;

    const auto files = *module_["file"].as_array();
    for(const auto& f: files){
        const auto& file = f.as_table();

        const auto& fileName = (*file)["name"].value<std::string>();
        fileNames.push_back(fileName.value());
    }
    const auto funcs = *module_["function"].as_array();
    for(const auto& f: funcs){
        const auto& func = f.as_table();
        const auto funcName = (*func)["name"].value<std::string>();
        funcNames.push_back(funcName.value());
    }

    return {moduleName.value(), fileNames};
}