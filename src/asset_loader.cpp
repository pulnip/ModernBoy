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

static std::optional<ScriptSection>
parseScriptSection(const toml::table*);


static std::tuple<std::string, std::vector<std::string>>
parseModule(const toml::table* ptr);
template<typename Component>
static std::optional<Component> parse(
    const toml::table*, AppState& app);

template<>
std::optional<RigidbodyComponent>
AssetLoader::parse<toml::table, RigidbodyComponent>(
    const toml::table* ptr
){
    if(ptr == nullptr)
        return std::nullopt;
    const auto& table = *ptr;
    auto comp = dangled<RigidbodyComponent>();

    if(auto vel = table["velocity"].as_array()){
        for(size_t i=0; i<3; ++i){
            comp.velocity.v[i] = *(*vel)[i].value<double>();
        }
    }
    comp.useGravity = table["useGravity"].value_or<bool>(false);
    comp.mass = table["mass"].value_or<double>(1);

    return comp;
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

template<typename T>
std::optional<T> parse(const toml::array* ptr);

template<>
std::optional<Vec3> parse<Vec3>(const toml::array* ptr){
    if(ptr == nullptr)
        return std::nullopt;
    const auto& v = *ptr;

    Vec3 vec;
    for(Index i=0; i<3; ++i)
        vec.v[i] = v[i].value<double>().value_or(0.0);
    return vec;
}
template<>
std::optional<Vec4> parse<Vec4>(const toml::array* ptr){
    if(ptr == nullptr)
        return std::nullopt;
    const auto& v = *ptr;

    Vec4 vec;
    for(Index i=0; i<4; ++i)
        vec.v[i] = v[i].value<double>().value_or(0.0);
    return vec;
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

    auto component = dangled<TransformComponent>();
    component.position = parse<Vec3>(
        table["position"].as_array()).value_or(zeros());
    component.rotation = parse<Vec4>(
        table["rotation"].as_array()).value_or(unitQuat());
    component.scale = parse<Vec3>(
        table["scale"].as_array()).value_or(ones());

    return component;
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

template<>
std::optional<ScriptComponent>
parse<ScriptComponent>(
    const toml::table* ptr, AppState& app
){
    if(ptr == nullptr)
        return std::nullopt;
    const auto& script = *ptr;

    std::string moduleName = script["module"].value_or("component");
    std::string typeName = script["type"].value_or("IComponent");
    auto objectHandle = app.appendV2<Object>(moduleName, typeName);

    return dangled<ScriptComponent>(objectHandle);
}

static std::optional<ScriptSection>
parseScriptSection(const toml::table* ptr){
    if(ptr == nullptr)
        return std::nullopt;
    const auto& table = *ptr;

    auto moduleName = *table["module"].value<std::string>();

    return ScriptSection{
        .moduleName = moduleName
    };
}

std::optional<ActionComponent> 
AssetLoader::makeActionComponent(
    const ScriptSection& section
){
    auto component = dangled<ActionComponent>(
        app.moduleManager.getHandle(
            section.moduleName),
        app.scriptInvoker.registerFunction(
            "update")
    );

    return component;
}

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

template<>
std::optional<SphereColliderComponent>
parse<SphereColliderComponent>(
    const toml::table* ptr, AppState&
){
    if(ptr==nullptr)
        return std::nullopt;
    const auto& collider = *ptr;

    auto position = parse<Vec3>(
        collider["position"].as_array()).value_or(zeros());
    float radius = collider["radius"].value_or(1.0);

    auto component = dangled<SphereColliderComponent>(
        position, radius
    );

    return component;
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

void AssetLoader::loadAsset(const std::string& fileName){
    AppDebug("Load Asset: {}", fileName);
    auto table = toml::parse_file(fileName);

    auto entities = *table["entities"].as_array();

    for(const auto& ntt: entities){
        const auto& entity = *ntt.as_table();

        // new Actor
        std::string name = entity["name"].value<std::string>().value();

        auto tc = ::parse<TransformComponent>(
            entity["transform"].as_table(), app);
        auto cc = ::parse<CameraComponent>(
            entity["camera"].as_table(), app);

        std::optional<MeshComponent> mc;
        if(entity.contains("model")){
            auto model = entity["model"].as_table();

            mc = ::parse<MeshComponent>(model, app);
        }

        // auto ac = parseScriptSection(
        //     entity["script"].as_table()
        // ).and_then([this](auto&& val){
        //     return makeActionComponent(val); 
        // });
        auto sc = ::parse<ScriptComponent>(
            entity["script"].as_table(), app);

        auto ic = ::parse<InputComponent>(
            entity["script"].as_table(), app);

        auto rc = parse<toml::table, RigidbodyComponent>(
            entity["rigidbody"].as_table());

        auto scc = ::parse<SphereColliderComponent>(
            entity["sphereCollider"].as_table(), app);

        GameDebug("Actor loaded, name: {}", name);

        app.world.registry.createEntity(tc, cc, mc, sc, ic, rc,
            scc);
    }
}