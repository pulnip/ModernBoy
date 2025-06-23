#include <cassert>
#include <vector>
#include "app_state.hpp"
#include "input/state.hpp"

using namespace ModernBoy;

template<>
std::vector<RawMesh> ModernBoy::import<RawMesh>(
    AppState& app, const std::string& filename
){ return app.meshImporter.import(filename); }

template<>
MeshHandle ModernBoy::manage<Mesh>(
    AppState& app, Mesh&& resource
){ return app.meshManager.create(std::move(resource)); }

template<>
Mesh& ModernBoy::get<Mesh>(AppState& app,
    MeshHandle handle
){
    assert(handle.type==ResourceType::MESH);
    return *app.meshManager.get(handle);
}
template<>
Mesh& AppState::get<Mesh>(
    MeshHandle handle
){
    assert(handle.type==ResourceType::MESH);
    return *meshManager.get(handle);
}

template<>
Shader& ModernBoy::get<Shader>(AppState& app,
    ShaderHandle handle
){
    assert(handle.type==ResourceType::SHADER);
    return *app.shaderManager.get(handle);
}
template<>
Shader& AppState::get<Shader>(
    ShaderHandle handle
){
    assert(handle.type==ResourceType::SHADER);
    return *shaderManager.get(handle);
}

template<>
std::vector<RenderTask> AppState::get<RenderTask>(){
    std::vector<RenderTask> result;

    for(const auto& actorTask: renderSystem){
        result.append_range(actorTask.second);
    }
    return result;
}

template<>
std::vector<InputTask> AppState::get<InputTask>(
    const Input::State& state
){
    auto copied = inputSystem;
    std::vector<InputTask> result;

    for(auto& actorTask: copied){
        std::erase_if(actorTask.second,
            [&state](const auto& task){
                return task.condition != state.key[task.button];
            }
        );
    }
    for(auto& actorTask: copied){
        result.append_range(actorTask.second);
    }
    return result;
}

template<>
std::vector<ViewTask> AppState::get<ViewTask>(){
    std::vector<ViewTask> result;

    for(const auto& actorTask: viewSystem){
        result.append_range(actorTask.second);
    }
    return result;
}
