#include <cassert>
#include <chrono>
#include <format>
#include <vector>
#include "app_state.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace ModernBoy;


DeltaTime AppState::getDeltaTime() const{
    return scheduler.getDeltaTime();
}

AppState::AppState(SDL_Window* window)
:window(window),
// resource manager
meshManager(), textureManager(),
shaderManager(), moduleManager(),
// subsystems
renderer(window, meshManager, textureManager,
    shaderManager, world),
userInterface(window, renderer, *this),
inputChord(),
scriptInvoker(world, inputChord),
// others
scheduler(*this), world(*this),
assetLoader(*this), generators(){}
AppState::~AppState(){
    SDL_DestroyWindow(window);
}

EntityID AppState::issueID(){
    return id_seed++;
}

template<>
void AppState::on<Event::OnFrameStart>(){
    renderer.onFrameStart();
    userInterface.onFrameStart();

}
template<>
void AppState::on<Event::OnFrameEnd>(){
    userInterface.onFrameEnd();
    renderer.onFrameEnd();
}

template<>
MeshHandle AppState::append<Mesh, std::string&>
(std::string& meshFile){
    return meshManager.emplace(
        meshFile, renderer.context.metalLayer
    );
}
template<>
MeshHandle AppState::append<Texture, std::string&>
(std::string& textureFile){
    return textureManager.emplace(
        textureFile, renderer.context.metalLayer
    );
}
template<>
MeshHandle AppState::append<Shader, std::string>
(std::string&& shaderFile){
    return shaderManager.emplace(
        shaderFile, renderer.context.metalLayer
    );
}
template<>
MeshHandle AppState::append<Script::Module,
    std::string&, std::vector<std::string>&>
(std::string& moduleFile, std::vector<std::string>& funcs){
    
    return moduleManager.emplace(moduleFile, funcs,
        scriptInvoker.engine);
}


void AppState::prepare(){
    scheduler.prepareTaskPhase(renderer);
    scheduler.prepareTaskPhase(userInterface);
}

void AppState::update(){
    Timepoint now = std::chrono::time_point_cast<
        std::chrono::microseconds>(steady_clock::now());
    deltaTime = lastTick - now;
    lastTick = now;

    inputChord.update(now);

    world.update(deltaTime);

    // userInterface.update(now);
    // renderer.update(now);

    scheduler.prepareScheduling();

    scheduler.prepareFrame();

    scheduler.updateFrame();

    constexpr auto TARGET_FPS = 60;
    constexpr auto frameTime = (1000ms/TARGET_FPS) - 2ms;
    auto proceedTime = steady_clock::now() - now;

    if(proceedTime <= frameTime){
        std::this_thread::sleep_for(frameTime - proceedTime);
    }
}

template<> Mesh&
AppState::query(ResourceHandle handle){
    return meshManager.get(handle);
}
template<> Texture&
AppState::query(ResourceHandle handle){
    return textureManager.get(handle);
}
template<> Shader&
AppState::query(ResourceHandle handle){
    return shaderManager.get(handle);
}
template<> Script::Module&
AppState::query(ResourceHandle handle){
    return moduleManager.get(handle);
}

template<> ResourceHandle
AppState::query<Script::Module>(const std::string& name){
    return moduleManager.getHandle(name);
}

FunctionID AppState::registerFunction(
    const std::string& funcName
){
    return scriptInvoker.registerFunction(funcName);
}

NativePtr AppState::getRenderPassDesc(){
    return renderer.getRenderPassDesc();
}
NativePtr AppState::getDevice(){
    return renderer.getDevice();

}
NativePtr AppState::getCommandBuffer(){
    return renderer.getCommandBuffer();

}
NativePtr AppState::getRenderEncoder(){
    return renderer.getRenderEncoder();
}
