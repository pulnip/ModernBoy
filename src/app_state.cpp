#include <cassert>
#include <format>
#include <vector>
#include "app_state.hpp"
#include "input/state.hpp"

using namespace ModernBoy;

AppState::AppState(SDL_Window* window)
:gui(), meshImporter(), window(window), inputDevice(),
meshManager(*this), shaderManager(*this),
transformPool(), cameraPool(),
meshPool(), inputPool(),
renderSystem(), viewSystem(), inputSystem(),
// Important!! Initialize Order
controller(*this), renderer(window, *this),
assetLoader(*this){}
AppState::~AppState(){}

EntityID AppState::issueID(){
    return id_seed++;
}

static void assignEntityID(SparseChunk& components,
    EntityID actor);
static void linkActor(AppState& app, EntityID actor,
    const SparseChunk& linker, ArchetypeBit bit);

EntityID AppState::createActor(ArchetypeBit bit,
    SparseChunk&& components
){
    EntityID actor_id = issueID();
    assignEntityID(components, actor_id);

    auto chunkIndex = archetypeMap.insert(
        bit, components);
    auto [it, ret] = actorTable.emplace(
        actor_id, ComponentInfo{bit, chunkIndex});
    if(!ret){
        std::string actorInfo = std::format(
            "Actor{} type:{} Not Created!",
            actor_id, bit
        );
        throw std::runtime_error(actorInfo);
    }
    linkActor(*this, actor_id, components, bit);
    return actor_id;
}
void AppState::destroyActor(EntityID actor){
    ComponentInfo info = actorTable.at(actor);
    archetypeMap.at(info.bit).freeChunk(info.chunkIndex);
    actorTable.erase(actor);
}

static void assignEntityID(SparseChunk& components,
    EntityID actor
){
    components.transform.actor = actor;
    components.camera.actor = actor;
    components.mesh.actor = actor;
    components.input.actor = actor;
}

static void linkActor(AppState& app, EntityID actor,
    const SparseChunk& chunk, ArchetypeBit bit
){
    if(subset(bit, RENDER_BIT)){
        auto transform = chunk.transform.value;
        auto& meshHandles = app.meshManager.get(chunk.mesh.accessHandle);
        std::vector<RenderTask> tasks(meshHandles.size());
        for(size_t i=0; i<tasks.size(); ++i)
            tasks[i] = RenderTask{transform, meshHandles[i]};
        app.renderSystem.emplace(actor, tasks);
    }

    if(subset(bit, VIEW_BIT)){
        auto transform = chunk.transform.value;
        std::vector<ViewTask> tasks(1);
        tasks[0] = ViewTask{transform, chunk.camera.value};
        app.viewSystem.emplace(actor, tasks);
    }

    if(subset(bit, KB_IN_BIT)){
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

template<>
std::vector<Mesh> ModernBoy::import<Mesh>(
    AppState& app, const std::string& filename
){
    auto rawMeshes = app.meshImporter.import(filename);
    std::vector<Mesh> meshes(rawMeshes.size());
    for(auto& rawMesh: rawMeshes)
        meshes.emplace_back(rawMesh, app);
    return meshes;
}
