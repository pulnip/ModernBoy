#include <cassert>
#include <chrono>
#include <format>
#include <vector>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#if defined(USE_METAL)
#include <SDL3/SDL_metal.h>
#endif
#include "engine/asset/scene_parser.hpp"
#include "engine/engine.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace ModernBoy;
using namespace ModernBoy::Interface;

Engine ModernBoy::createEngine(){
    if(!SDL_SetAppMetadata("ModernBoy", "1.0", "com.example.game0")){
        throw SDL_APP_FAILURE;
    }
    if(!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        throw SDL_APP_FAILURE;
    }

    SDL_WindowFlags flags = SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS;

#if defined(USE_OPENGL)
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    flags |= SDL_WINDOW_OPENGL;
#endif

    auto window = SDL_CreateWindow("examples/demo/game0", 800, 600, flags);
    if(window == nullptr){
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        throw SDL_APP_FAILURE;
    }

    return Engine(window);
}

DeltaTime Engine::getDeltaTime() const{
    return deltaTime;
}

Engine::Engine(SDL_Window* window)
:window(window),
// resource manager
meshManager(), textureManager(),
shaderManager(), moduleManager(),
// subsystems
renderer(window, submeshManager, materialManager, meshManager, textureManager,
    shaderManager, world, viewService, drawService),
userInterface(window, renderer, *this),
inputDevice(window, inputService),
scriptInvoker(world.registry, moduleManager,
    objectManager, *this),
// others
world(*this),
assetLoader(*this),
assetLoader2(meshTable, materialSetTable,
    submeshManager, materialManager,
    shaderManager, renderer.context),
sceneLoader(world.registry, shaderManager),
lastTick(std::chrono::time_point_cast<
    std::chrono::microseconds>(steady_clock::now())){}

void Engine::start(){
    auto temp = Asset::parseSceneFromFile("asset/scene.toml");

    auto registry = Asset::makeDefaultBinderRegistry();
    auto scene = Asset::buildScene(temp, registry);

    assetLoader2.load(scene);
    sceneLoader.loadScene(scene, assetLoader2.get_table());
}

void Engine::shutdown(){
    SDL_DestroyWindow(window);
    window = nullptr;
}

EntityID Engine::issueID(){
    return id_seed++;
}

template<>
void Engine::on<Event::OnFrameStart>(){
    renderer.onFrameStart();
    userInterface.onFrameStart();

}
template<>
void Engine::on<Event::OnFrameEnd>(){
    userInterface.onFrameEnd();
    renderer.onFrameEnd();
}

template<>
MeshHandle Engine::append<Mesh, const std::string&>
(const std::string& meshFile){
#if defined(USE_DIRECTX)
    return meshManager.emplace(meshFile);
#elif defined(USE_METAL)
    return meshManager.emplace(
        issueID(), renderer.context, meshFile
    );
#endif
}
template<>
TextureHandle Engine::append<Texture, const std::string&>
(const std::string& textureFile){
#if defined(USE_DIRECTX)
    return textureManager.emplace(textureFile);
#elif defined(USE_METAL)
    return textureManager.emplace(
        issueID(), renderer.context, textureFile 
    );
#endif
}
template<>
ShaderHandle Engine::append<Shader, const std::string&, const std::string&>
(const std::string& vsFuncName, const std::string& fsFuncName){
#if defined(USE_DIRECTX)
    return shaderManager.emplace(shaderFile);
#elif defined(USE_METAL)
    return shaderManager.emplace(issueID(), renderer.context,
        vsFuncName, fsFuncName
    );
#endif
}
template<>
ModuleHandle Engine::append<Module,
    const std::string&, const std::vector<std::string>&>
(const std::string& moduleFile, const std::vector<std::string>& funcs){
    return moduleManager.emplace(moduleFile, moduleFile, funcs,
        scriptInvoker.scriptEngine);
}
template<>
ObjectHandle Engine::append<Object, const std::string&, const std::string&>
(const std::string& moduleName, const std::string& typeName){

    return objectManager.emplace(
        issueID(), typeName,
        moduleManager.get(moduleName).module_,
        scriptInvoker.scriptEngine);
}


void Engine::update(){
    Timepoint now = std::chrono::time_point_cast<
        std::chrono::microseconds>(steady_clock::now());
    deltaTime = now - lastTick;
    lastTick = now;

    auto commands = engineCommandBus.drainCommands();
    for(const auto& cmd: commands)
        std::visit([this](auto const& c){
            (*this)(c);
        }, cmd);

    inputDevice.update(deltaTime);
    // auto gen1 = inputDevice.update(deltaTime);
    // while(!gen1.done())
    //     gen1.next();

    world.update(deltaTime);

    on<Event::OnFrameStart>();
    userInterface.update(deltaTime);
    // auto gen3 = userInterface.update(deltaTime);
    // while(!gen3.done())
    //     gen3.next();
    renderer.update(deltaTime);
    // auto gen4 = renderer.update(deltaTime);
    // while(!gen4.done())
    //     gen4.next();
    on<Event::OnFrameEnd>();

    constexpr auto TARGET_FPS = 60;
    constexpr auto frameTime = (1000ms/TARGET_FPS) - 2ms;
    auto proceedTime = steady_clock::now() - now;

    if(proceedTime <= frameTime){
        std::this_thread::sleep_for(frameTime - proceedTime);
    }
}

template<> Mesh&
Engine::query(Handle handle){
    return meshManager.get(handle);
}
template<> Texture&
Engine::query(Handle handle){
    return textureManager.get(handle);
}
template<> Shader&
Engine::query(Handle handle){
    return shaderManager.get(handle);
}
template<> Script::Module&
Engine::query(Handle handle){
    return moduleManager.get(handle);
}

Script::FunctionID Engine::registerFunction(
    const std::string& funcName
){
    return scriptInvoker.registerFunction(funcName);
}

#if defined(USE_DIRECTX)
NativePtr Engine::getDevice(){
    return renderer.getDevice();
}
NativePtr Engine::getContext(){
    return renderer.getContext();
}
#elif defined(USE_METAL)
NativePtr Engine::getRenderPassDesc(){
    return renderer.getRenderPassDesc();
}
NativePtr Engine::getDevice(){
    return renderer.getDevice();
}
NativePtr Engine::getCommandBuffer(){
    return renderer.getCommandBuffer();
}
NativePtr Engine::getRenderEncoder(){
    return renderer.getRenderEncoder();
}
#endif

void Engine::operator()(SetMouseMode cmd){
    inputDevice.setMouseMode(cmd.isRelative);
}
