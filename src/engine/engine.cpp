#include <cassert>
#include <chrono>
#include <format>
#include <vector>
#include <SDL3/SDL_init.h>
#include "engine/engine.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace ModernBoy;

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
renderer(window, meshManager, textureManager,
    shaderManager, world, debugDrawService),
userInterface(window, renderer, *this),
inputDevice(window, inputService),
scriptInvoker(world.registry, moduleManager,
    objectManager, *this),
// others
world(*this),
assetLoader(*this), lastTick(std::chrono::time_point_cast<
    std::chrono::microseconds>(steady_clock::now())){}

void Engine::shutdown(){
    objectManager.clear();
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
        meshFile, meshFile, renderer.metalLayer
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
        textureFile, textureFile, renderer.metalLayer
    );
#endif
}
template<>
ShaderHandle Engine::append<Shader, const std::string&, const std::string&>
(const std::string& vsFuncName, const std::string& fsFuncName){
#if defined(USE_DIRECTX)
    return shaderManager.emplace(shaderFile);
#elif defined(USE_METAL)
    return shaderManager.emplace(fsFuncName, renderer.context,
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
ObjectHandle Engine::appendV2<Object, const std::string&, const std::string&>
(const std::string& moduleName, const std::string& typeName){

    return objectManager.emplace(
        std::format("{}{}", typeName, issueID()), typeName,
        moduleManager.get(moduleName).module_,
        scriptInvoker.scriptEngine);
}


void Engine::update(){
    Timepoint now = std::chrono::time_point_cast<
        std::chrono::microseconds>(steady_clock::now());
    deltaTime = now - lastTick;
    lastTick = now;

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
Engine::query(ResourceHandle handle){
    return meshManager.get(handle);
}
template<> Texture&
Engine::query(ResourceHandle handle){
    return textureManager.get(handle);
}
template<> Shader&
Engine::query(ResourceHandle handle){
    return shaderManager.get(handle);
}
template<> Script::Module&
Engine::query(ResourceHandle handle){
    return moduleManager.get(handle);
}

template<> ResourceHandle
Engine::query<Script::Module>(const std::string& name){
    return moduleManager.getHandle(name);
}

FunctionID Engine::registerFunction(
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
