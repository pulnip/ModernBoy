#include <cassert>
#include <vector>
#include "app_state.hpp"
#include "input/state.hpp"

using namespace ModernBoy;

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
        break; // TODO: Multi View
    }
    return result;
}
