#include <vector>
#include "app_state.hpp"

using namespace ModernBoy;

template<>
std::vector<RawMesh> ModernBoy::import<RawMesh>(
    AppState& app, const std::string& filename
){ return app.meshImporter.import(filename); }

template<>
ResourceHandle<Mesh> ModernBoy::manage<Mesh>(
    AppState& app, Mesh&& resource
){ return app.meshManager.create(std::move(resource)); }

template<>
Transform& ModernBoy::get<Transform>(AppState& app,
    ResourceHandle<Transform> handle
){ return *app.transformManager.get(handle); }
template<>
Transform& AppState::get<Transform>(
    ResourceHandle<Transform> handle
){ return *transformManager.get(handle); }

template<>
Mesh& ModernBoy::get<Mesh>(AppState& app,
    ResourceHandle<Mesh> handle
){ return *app.meshManager.get(handle); }
template<>
Mesh& AppState::get<Mesh>(
    ResourceHandle<Mesh> handle
){ return *meshManager.get(handle); }

template<>
Shader& ModernBoy::get<Shader>(AppState& app,
    ResourceHandle<Shader> handle
){ return *app.shaderManager.get(handle); }
template<>
Shader& AppState::get<Shader>(
    ResourceHandle<Shader> handle
){ return *shaderManager.get(handle); }

template<>
Camera& ModernBoy::get<Camera>(AppState& app,
    ResourceHandle<Camera> handle
){ return *app.cameraManager.get(handle); }
template<>
Camera& AppState::get<Camera>(
    ResourceHandle<Camera> handle
){ return *cameraManager.get(handle); }

template<>
std::vector<RenderTask> AppState::get<RenderTask>(){
    return renderSystem.getAll(); }

template<>
std::vector<ViewTask> AppState::get<ViewTask>(){
    return viewSystem.getAll(); }

template<>
std::vector<InputTask> AppState::get<InputTask>(){
    return inputSystem.getAll(); }
