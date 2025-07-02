#include <cassert>
#include <format>
#include <vector>
#include "app_state.hpp"

using namespace ModernBoy;

Uint64 AppState::getDeltaTime() const{ return deltaTime; }

AppState::AppState(SDL_Window* window)
// :archetypeMaps({ArchetypeMapV2{}, ArchetypeMapV2{}}),
// moduleManagerV2(*this), scheduler(*this),
:scheduler(*this),
ui(), window(window), inputDevice(),
// Important!! Initialize Order
meshManager(*this), textureManager(*this),
shaderManager(*this), moduleManager(*this),
renderSystem(*this), inputSystem(*this),
scriptInvoker(*this),
assetLoader(*this){}
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

template<> Mesh& AppState::get(ResourceHandle handle
){ return meshManager.get(handle); }
template<> Mesh& ModernBoy::get(AppState& app, 
    ResourceHandle handle
){ return app.get<Mesh>(handle); }

template<> Texture& AppState::get(ResourceHandle handle
){ return textureManager.get(handle); }
template<> Texture& ModernBoy::get(AppState& app,
    ResourceHandle handle
){ return app.get<Texture>(handle); }

template<> Shader& AppState::get(ResourceHandle handle
){ return shaderManager.get(handle); }
template<> Shader& ModernBoy::get(AppState& app,
    ResourceHandle handle
){ return app.get<Shader>(handle); }

template<> Script::Module& AppState::get(
    ResourceHandle handle
){ return moduleManager.get(handle); }
template<> Script::Module& ModernBoy::get(
    AppState& app, ResourceHandle handle
){ return app.get<Script::Module>(handle); }


template<> Script::Module& AppState::get(
    const std::string& name
){ return moduleManager.get(name); }
template<> Script::Module& ModernBoy::get(
    AppState& app, const std::string& name
){ return app.get<Script::Module>(name); }
template<> ResourceHandle AppState::getHandle<Script::Module>(
    const std::string& name
){ return moduleManager.getHandle(name); }
template<> ResourceHandle ModernBoy::getHandle<Script::Module>(
    AppState& app, const std::string& name
){ return app.getHandle<Script::Module>(name); }


template<typename Component>
std::optional<Component> AppState::query(EntityID actor){
    const auto& comp = actorTable.at(actor);
    auto querybit = bit_of<Component>;
    if((comp.bit & querybit) != querybit)
        return std::nullopt;
    Component c;
    archetypeMap.at(comp.bit).get(comp.chunkIndex, &c);
}
template<> std::optional<TransformComponent>
AppState::query<>(EntityID);
template<> std::optional<CameraComponent> AppState::query<>(EntityID);
template<> std::optional<MeshComponent>
AppState::query<>(EntityID);
template<> std::optional<InputComponent>
AppState::query<>(EntityID);

template<typename Component>
std::optional<Component> ModernBoy::query(
    AppState& app, EntityID actor
){ app.query<Component>(actor); }
template<> std::optional<TransformComponent>
ModernBoy::query<>(AppState&, EntityID);
template<> std::optional<CameraComponent>
ModernBoy::query<>(AppState&, EntityID);
template<> std::optional<MeshComponent>
ModernBoy::query<>(AppState&, EntityID);
template<> std::optional<InputComponent>
ModernBoy::query<>(AppState&, EntityID);
