#include <cassert>
#include <format>
#include <vector>
#include "app_state.hpp"

using namespace ModernBoy;

AppState::AppState(SDL_Window* window)
:gui(), window(window), inputDevice(),
// Important!! Initialize Order
meshManager(*this), shaderManager(*this),
moduleManager(*this),
renderSystem(*this, window), inputSystem(*this),
scriptInvoker(*this), assetLoader(*this){}
AppState::~AppState(){}

EntityID AppState::issueID(){
    return id_seed++;
}

static void assignEntityID(SparseChunk& components,
    EntityID actor);

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
    return actor_id;
}
void AppState::destroyActor(EntityID actor){
    ComponentInfo info = actorTable.at(actor);
    archetypeMap.at(info.bit).free(info.chunkIndex);
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
