#include <toml++/toml.h>
#include "asset_loader.hpp"
#include "app_state.hpp"
#include "resource_handle.hpp"
#include "component.hpp"

using namespace ModernBoy;

AssetLoader::AssetLoader(AppState& app):app(app){}

static Transform parseTransform(const toml::v3::table& table);
static Camera parseCamera(const toml::v3::table& table);

EntityID AssetLoader::issueID(){
    return id_seed++;
}

struct SparseChunk{
    TransformComponent transform;
    CameraComponent camera;
    MeshComponent mesh;
    InputComponent input;
};
static void linkActor(AppState& app, EntityID actor,
    const SparseChunk& linker, ArchetypeBit bit);

void AssetLoader::loadActors(const std::string& fileName){
    toml::table tbl = toml::parse_file(fileName);

    auto actors = *tbl["entities"].as_array();
    for(const auto& actor_node: actors){
        const auto& actor = *actor_node.as_table();
        // new Actor
        EntityID actor_id = issueID();
        ArchetypeBit bit = 0;
        SparseChunk chunk;

        // transform component
        if(auto trans_tbl = actor["transform"].as_table()){
            auto transform = parseTransform(*trans_tbl);

            chunk.transform = TransformComponent{actor_id, transform};
            bit = bit | TRANSFORM_BIT;
        }

        // mesh & texture & shader component
        if(auto model_arr = actor["model"].as_array()){
            for(const auto& parts_node: *model_arr){
                const auto& parts = *parts_node.as_table();

                // mesh component
                const auto meshFile = *parts["mesh"].value<std::string>();
                auto meshHandles = app.meshLoader.load(meshFile);
                chunk.mesh = MeshComponent{actor_id, meshHandles};
                bit = bit | MESH_BIT;

                // ToDo. texture component
                // const auto& texFile = *parts["diffuse"].value<std::string>();
                // ToDo. How to handle multiple model?
                break;
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
            chunk.input = InputComponent{actor_id, inputMap};
            bit = bit | INPUT_BIT;
        }
        app.actorTable.emplace(std::pair{actor_id, bit});
        linkActor(app, actor_id, chunk, bit);
    }
}
void AssetLoader::loadCamera(const std::string& fileName){
    toml::table tbl = toml::parse_file(fileName);

    auto cameras = tbl["entities"].as_array();

    for(const auto& cam_node: *cameras){
        const auto& cam = *cam_node.as_table();
        // new Actor
        EntityID actor_id = issueID();
        ArchetypeBit bit = 0;
        SparseChunk chunk;

        auto name = *cam["name"].value<std::string>();
        auto type = *cam["type"].value<std::string>();
        bool enabled = type.compare("MainCamera")==0;
        std::println("{} {}", name, type);

        if(auto t = cam["transform"].as_table()){
            auto transform = parseTransform(*t);

            chunk.transform = TransformComponent{actor_id, transform};
            bit = bit | TRANSFORM_BIT;
        }
        else{
            // warning!
        }
        if(auto c = cam["camera"].as_table()){
            auto camera = parseCamera(*c);
            chunk.camera = CameraComponent{actor_id, camera};
            bit = bit | CAMERA_BIT;
        }
        else{
            // warning!
        }
        app.actorTable.emplace(std::pair{actor_id, bit});
        linkActor(app, actor_id, chunk, bit);
        // ToDo. Not Accept Multiple Camera actor.
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
    const SparseChunk& chunk, ArchetypeBit bit
){
    if(bit & (TRANSFORM_BIT | MESH_BIT)){
        auto transform = chunk.transform.value;
        std::vector<RenderTask> tasks(chunk.mesh.meshHandles.size());
        for(size_t i=0; i<tasks.size(); ++i)
            tasks[i] = RenderTask{transform, chunk.mesh.meshHandles[i]};
        app.renderSystem.emplace(actor, tasks);
    }

    if(bit & (TRANSFORM_BIT | CAMERA_BIT)){
        auto transform = chunk.transform.value;
        std::vector<ViewTask> tasks(1);
        tasks[0] = ViewTask{transform, chunk.camera.value};
        app.viewSystem.emplace(actor, tasks);
    }

    if(bit & (TRANSFORM_BIT | INPUT_BIT)){
        auto transform = chunk.transform.value;
        std::vector<InputTask> tasks;
        for(const auto& buttonMap: chunk.input.value){
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
