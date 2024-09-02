#include <print>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_sdl2.h>
#include <imgui.h>
#include "Actor.hpp"
#include "CameraComponent.hpp"
#include "Core.hpp"
#include "helper.hpp"
#include "GUIAdaptor.hpp"
#include "MeshComponent.hpp"
#include "RenderAdaptor.hpp"
#include "ShaderAdaptor.hpp"
#include "TextureAdaptor.hpp"
#include "Window.hpp"

using namespace std;
using namespace DirectX;
using namespace ModernBoy;

namespace ModernBoy{
    [[nodiscard]] inline SDL_Window* createWindow(
        const WindowDesc& wd
    ){
        return SDL_CreateWindow(
            wd.title.c_str(),
            wd.position.x, wd.position.y,
            wd.size.x, wd.size.y,
            // 0
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        );
    }
    
    [[nodiscard]] inline HWND getHandle(SDL_Window* window){
        SDL_SysWMinfo wmInfo; SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window, &wmInfo);

        return wmInfo.info.win.window;
    }
    
    [[nodiscard, maybe_unused]]
    inline std::string getRendererName(SDL_Renderer* renderer){
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer, &info);

        return info.name;
    }
}

Window::Window([[maybe_unused]] const Core& core,
    const WindowDesc& wd
){
    constexpr auto IMG_FLAGS = IMG_INIT_JPG|IMG_INIT_PNG;
        throwIfTrue(IMG_Init(IMG_FLAGS) != IMG_FLAGS, IMG_GetError());
#ifdef SDL_HINT_IME_SHOW_UI
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    throwIfTrue((window=createWindow(wd))==nullptr,
        "SDL_CreateWindow() Failed."
    );

    HWND hwnd=getHandle(window);
    renderer=make_unique<RenderAdaptor>(hwnd, wd.size);
    texturer=make_unique<TextureAdaptor>(renderer->device);
    shader=make_unique<ShaderAdaptor>(renderer->device,
        L"src/VS.hlsl", L"src/RimPS.hlsl"
    );

    gui=make_unique<GUIAdaptor>(window,
        renderer->device, renderer->context,
        wd.size
    );
}

Window::~Window(){
    SDL_DestroyWindow(window);
    IMG_Quit();
}

void Window::process(const SDL_WindowEvent& e){
    if(e.windowID!=SDL_GetWindowID(window))
        return;

    switch(e.event){
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        const auto width=e.data1;
        const auto height=e.data2;
        shader->mainCamera->setScreenSize(width, height);
        // break;
        renderer->recreateRenderTarget(width, height);
        break;
    }
}

void Window::update(){
    gui->update();
    deltaTime=gui->deltaTime;

    shader->mainCamera->setPerspective(XMConvertToRadians(gui->fov));

    shader->lights=lights;
    shader->lightType=gui->lightType;
    shader->rimStrength=gui->rimStrength;
    shader->rimPower=gui->rimPower;
    if(gui->normalScale != 0.0f){
        shader->normalScale=gui->normalScale;
    }

    // Rendering
    renderer->setup();
    for(const auto& mesh: meshes){
        mesh->draw(*shader, *texturer, renderer->context);
    }
    shader->setWireFrame(gui->drawAsWire, renderer->context);
    shader->render(renderer->context);
    if(gui->normalScale != 0.0f){
        shader->renderNormal(renderer->context);
    }
    gui->render();

    renderer->swap();
}

bool Window::isMinimized(){
    return SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED;
}

// void Window::update([[maybe_unused]] float dt){
//     mainCamera->setEyePos({0.0f, 0.0f, -1.0f});
//     mainCamera->setEyeDir(Vector3::UnitZ);
//     mainCamera->setUpDir(Vector3::UnitY);
// }

void Window::setMainCamera(shared_ptr<CameraComponent> camera){
    shader->mainCamera=camera;
}

void Window::addLight(shared_ptr<LightComponent> light){
    lights.emplace_back(light);
}

void Window::addMesh(shared_ptr<MeshComponent> mesh){
    meshes.emplace_back(mesh);
    shader->loadMesh(mesh->mesh, renderer->device);
    shader->loadNormal(mesh->mesh, renderer->device);
}
