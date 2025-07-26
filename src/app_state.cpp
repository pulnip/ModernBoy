#include <cassert>
#include <chrono>
#include <format>
#include <vector>
#include <SDL3/SDL_init.h>
#include "app_state.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace ModernBoy;

AppState ModernBoy::createAppState(){
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

    return AppState(window);
}

DeltaTime AppState::getDeltaTime() const{
    return deltaTime;
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
scriptInvoker(world.registry, moduleManager,
    objectManager, inputChord),
// others
world(*this),
assetLoader(*this), lastTick(std::chrono::time_point_cast<
    std::chrono::microseconds>(steady_clock::now())){}

void AppState::shutdown(){
    objectManager.clear();
    SDL_DestroyWindow(window);
    window = nullptr;
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
#if defined(USE_DIRECTX)
    return meshManager.emplace(meshFile);
#elif defined(USE_METAL)
    return meshManager.emplace(
        meshFile, renderer.context.metalLayer
    );
#endif
}
template<>
TextureHandle AppState::append<Texture, std::string&>
(std::string& textureFile){
#if defined(USE_DIRECTX)
    return textureManager.emplace(textureFile);
#elif defined(USE_METAL)
    return textureManager.emplace(
        textureFile, renderer.context.metalLayer
    );
#endif
}
template<>
ShaderHandle AppState::append<Shader, std::string>
(std::string&& shaderFile){
#if defined(USE_DIRECTX)
    return shaderManager.emplace(shaderFile);
#elif defined(USE_METAL)
    return shaderManager.emplace(
        shaderFile, renderer.context.metalLayer
    );
#endif
}
template<>
ModuleHandle AppState::append<Module,
    std::string&, std::vector<std::string>&>
(std::string& moduleFile, std::vector<std::string>& funcs){
    return moduleManager.emplace(moduleFile, funcs,
        scriptInvoker.engine);
}
template<>
ObjectHandle AppState::appendV2<Object, std::string&, std::string&>
(std::string& moduleName, std::string& typeName){

    return objectManager.emplace(
        std::format("{}{}", typeName, issueID()), typeName,
        moduleManager.get(moduleName).module_,
        scriptInvoker.engine);
}


void AppState::update(){
    Timepoint now = std::chrono::time_point_cast<
        std::chrono::microseconds>(steady_clock::now());
    deltaTime = now - lastTick;
    lastTick = now;

    inputChord.update(deltaTime);
    // auto gen1 = inputChord.update(deltaTime);
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

#if defined(USE_DIRECTX)
NativePtr AppState::getDevice(){
    return renderer.getDevice();
}
NativePtr AppState::getContext(){
    return renderer.getContext();
}
#elif defined(USE_METAL)
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
#endif
