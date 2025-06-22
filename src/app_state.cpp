#include <cassert>
#include <vector>
#include "app_state.hpp"

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
Transform& ModernBoy::get<Transform>(AppState& app,
    TransformHandle handle
){
    assert(handle.type==ResourceType::TRANSFORM);
    return *app.transformManager.get(handle);
}
template<>
Transform& AppState::get<Transform>(
    TransformHandle handle
){
    assert(handle.type==ResourceType::TRANSFORM);
    return *transformManager.get(handle);
}

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
Camera& ModernBoy::get<Camera>(AppState& app,
    CameraHandle handle
){
    assert(handle.type==ResourceType::CAMERA);
    return *app.cameraManager.get(handle);
}
template<>
Camera& AppState::get<Camera>(
    CameraHandle handle
){
    assert(handle.type==ResourceType::CAMERA);
    return *cameraManager.get(handle);
}

template<>
std::vector<RenderTask> AppState::get<RenderTask>(){
    return renderSystem.getAll(); }

template<>
std::vector<ViewTask> AppState::get<ViewTask>(){
    return viewSystem.getAll(); }

template<>
std::vector<InputTask> AppState::get<InputTask>(){
    return inputSystem.getAll(); }
