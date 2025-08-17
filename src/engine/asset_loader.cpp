#include <optional>
#include <regex>
#if defined(USE_DIRECTX)
#elif defined(USE_METAL)
#include <SDL3/SDL_metal.h>
#elif defined(USE_OPENGL)
#endif
#include <toml++/toml.h>
#include "core/math/type.hpp"
#include "core/string/helper.hpp"
#include "engine/log.hpp"
#include "engine/asset_loader.hpp"
#include "engine/engine.hpp"
#include "../../src/game/component.hpp"
#include "engine/script/invoker.hpp"
#include "engine/script/type.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

template<typename T>
static std::optional<T> parse(toml::node_view<const toml::node>);
template<typename T>
static std::optional<T> parse(const toml::node&);

#define CHECK_IF_ARRAY(x, out, n) \
    if(!x.is_array()) \
        return std::nullopt; \
    auto ptr = x.as_array(); \
    if(ptr == nullptr) \
        return std::nullopt; \
    auto& out = *ptr; \
    if(out.size() != n) \
        return std::nullopt;
#define CHECK_IF_TABLE(x, out) \
    if(!x.is_table()) \
        return std::nullopt; \
    auto ptr = x.as_table(); \
    if(ptr == nullptr) \
        return std::nullopt; \
    auto& out = *ptr;

template<>
std::optional<Vec3> parse(toml::node_view<const toml::node> view){
    CHECK_IF_ARRAY(view, arr, 3)

    Vec3 vec;
    for(size_t i=0; i<arr.size(); ++i){
        vec.v[i] = arr[i].value_or(0.0);
    }

    return vec;
}

template<>
std::optional<Vec4> parse(toml::node_view<const toml::node> view){
    CHECK_IF_ARRAY(view, arr, 4)

    Vec4 vec;
    for(size_t i=0; i<arr.size(); ++i){
        vec.v[i] = arr[i].value_or(0.0);
    }

    return vec;
}

template<>
std::optional<std::vector<std::string>> parse(toml::node_view<const toml::node> view){
    if(!view.is_array())
        return std::nullopt;

    auto ptr = view.as_array();
    if(ptr == nullptr)
        return std::nullopt;

    auto& vec = *ptr;
    std::vector<std::string> stringvec;
    stringvec.reserve(vec.size());

    for(size_t i=0; i<vec.size(); ++i){
        stringvec.emplace_back(vec[i].value<std::string>().value());
    }
    return stringvec;
}

EntityID invalidEntityID(){ return std::numeric_limits<EntityID>::max(); }

template<>
std::optional<Transform> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return Transform{
        .entity = invalidEntityID(),
        .isActive = true,
        .position = parse<Vec3>(table["position"]).value_or(zeros()),
        .rotation = parse<Vec4>(table["rotation"]).value_or(unitQuat()),
        .scale = parse<Vec3>(table["scale"]).value_or(ones()),
    };
}

template<>
std::optional<ModelDescriptor> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return ModelDescriptor{
        .mesh = table["mesh"].value_or<std::string>("cube"),
        .texture = table["texture"].value_or<std::string>("asset/metal_logo.png.png"),
        .vsFunc = table["vsFunc"].value_or<std::string>("vertex_main"),
        .fsFunc = table["fsFunc"].value_or<std::string>("fragment_main")
    };
}
Model AssetLoader::load(const ModelDescriptor& md){
    auto mesh = engine.appendV2<Mesh>(md.mesh);
    auto texture = engine.appendV2<Texture>(md.texture);
    auto shader = engine.appendV2<Shader>(md.vsFunc, md.fsFunc);

    return Model{
        .entity = invalidEntityID(),
        .isActive = true,
        .meshHandle = mesh,
        .alpha = 1.0,
        .textureHandle = texture,
        .shaderHandle = shader,
    };
}

template<>
std::optional<ScriptDescriptor> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return ScriptDescriptor{
        .module_ = table["module"].value_or<std::string>("component"),
        .type = table["type"].value_or<std::string>("IComponent")
    };
}
Game::ScriptObject AssetLoader::load(const ScriptDescriptor& sd){
    auto objectHandle = engine.appendV2<Object>(sd.module_, sd.type);
    return Game::ScriptObject{
        .entity = invalidEntityID(),
        .isActive = true,
        .handle = objectHandle
    };
}

template<>
std::optional<Rigidbody> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return Rigidbody{
        .entity = invalidEntityID(),
        .isActive = true,
        .velocity = parse<Vec3>(table["velocity"]).value_or(zeros()),
        .useGravity = table["useGravity"].value_or(false),
        .mass = static_cast<float>(table["mass"].value_or(1.0))
    };
}

template<>
std::optional<PhysicsMaterial> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return PhysicsMaterial{
        .bounciness = static_cast<float>(table["bounciness"].value_or(1.0)),
        .friction = static_cast<float>(table["friction"].value_or(1.0))
    };
}

template<>
std::optional<SphereCollider> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return SphereCollider{
        .entity = invalidEntityID(),
        .isActive = true,
        .position = parse<Vec3>(table["position"]).value_or(zeros()),
        .radius = static_cast<float>(table["radius"].value_or(1.0)),
        .material = parse<PhysicsMaterial>(table["material"]).value_or(
            PhysicsMaterial{.bounciness = 1.0f, .friction = 1.0f}
        )
    };
}
template<>
std::optional<BoxCollider> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return BoxCollider{
        .entity = invalidEntityID(),
        .isActive = true,
        .position = parse<Vec3>(table["position"]).value_or(zeros()),
        .rotation = parse<Vec4>(table["rotation"]).value_or(unitQuat()),
        .scale = parse<Vec3>(table["scale"]).value_or(zeros()),
        .material = parse<PhysicsMaterial>(table["material"]).value_or(
            PhysicsMaterial{.bounciness = 1.0f, .friction = 1.0f}
        )
    };
}

template<>
std::optional<Camera> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return Camera{
        .entity = invalidEntityID(),
        .isActive = true,
        .type = cameraType(table["type"].value_or("MainCamera")),
        .fov = static_cast<float>(table["fov"].value_or(100.0) * M_PI / 180),
        .nearPlane = static_cast<float>(table["nearPlane"].value_or(0.1)),
        .farPlane = static_cast<float>(table["farPlane"].value_or(0.1)),
        .projection = projection(table["projection"].value_or("perspective"))
    };
}

template<>
std::optional<Player> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return Player{
        .entity = invalidEntityID(),
        .isActive = true
    };
}
template<>
std::optional<Editor> parse(toml::node_view<const toml::node> view){
    CHECK_IF_TABLE(view, table)

    return Editor{
        .entity = invalidEntityID(),
        .isActive = true
    };
}

AssetLoader::AssetLoader(Engine& engine):engine(engine){
    loadAction("asset/action.toml");
    loadAsset("asset/actor.toml");
}

void AssetLoader::loadAsset(const std::string& fileName){
    AppDebug("Load Asset: {}", fileName);
    auto table = toml::parse_file(fileName);

    auto entities = *table["entities"].as_array();

    for(const auto& ntt: entities){
        const auto& entity = *ntt.as_table();

        // new Actor
        std::string name = entity["name"].value<std::string>().value();

        auto tf = parse<Transform>(entity["transform"]);
        auto cam = parse<Camera>(entity["camera"]);

        auto md = parse<ModelDescriptor>(entity["model"]);
        std::optional<Model> model = std::nullopt;
        if(md.has_value())
            model = load(md.value());

        auto sd = parse<ScriptDescriptor>(entity["script"]);
        std::optional<ScriptObject> so = std::nullopt;
        if(sd.has_value())
            so = load(sd.value());

        auto rb = parse<Rigidbody>(entity["rigidbody"]);
        auto sc = parse<SphereCollider>(
            entity["sphereCollider"]);
        auto bc = parse<BoxCollider>(
            entity["boxCollider"]);

        auto pc = parse<Player>(entity["player"]);
        auto ec = parse<Editor>(entity["editor"]);

        GameDebug("Actor loaded, name: {}", name);

        engine.world.registry.createEntity(tf, cam, model, so,
            rb, sc, bc,
            pc, ec);
    }
}

template<>
std::optional<ModuleDescriptor> parse(const toml::node& node){
    if(!node.is_table())
        return std::nullopt;

    auto ptr = node.as_table();
    if(ptr == nullptr)
        return std::nullopt;

    const auto& table = *ptr;

    return ModuleDescriptor{
        .module_ = table["name"].value<std::string>().value(),
        .files = parse<std::vector<std::string>>(
            table["file"]).value(),
    };
}

void AssetLoader::load(const ModuleDescriptor& md){
    engine.appendV2<Script::Module>(
        md.module_, md.files);
}

void AssetLoader::loadAction(const std::string& fileName){
    AppDebug("Load Action: {}", fileName);
    auto tbl = toml::parse_file(fileName);

    auto modules = *tbl["module"].as_array();
    for(const auto& md: modules)
        load(parse<ModuleDescriptor>(md).value());
}
