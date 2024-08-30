#include <print>
#include <SDL2/SDL.h>
#include <imgui_impl_sdl2.h>
#include "Actor.hpp"
#include "ActorManager.hpp"
#include "CameraComponent.hpp"
#include "CameraMoveComponent.hpp"
#include "Core.hpp"
#include "CubeMeshComponent.hpp"
#include "CylinderMeshComponent.hpp"
#include "GridMeshComponent.hpp"
#include "helper.hpp"
#include "LevelLoader.hpp"
#include "LightComponent.hpp"
#include "RenderAdaptor.hpp"
#include "SDL_InputSystem.hpp"
#include "SphereMeshComponent.hpp"
#include "TriangleMeshComponent.hpp"
#include "Window.hpp"

using namespace std;
using namespace ModernBoy;

Core::Core(){
    println("start core ctor...");
    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
    constexpr auto SDL_FLAGS = SDL_INIT_VIDEO|SDL_INIT_EVENTS;
    SDL_throwIf(SDL_Init(SDL_FLAGS));

    LevelLoader levelLoader("settings/glob.json");
    auto wd=levelLoader.getWindowDesc();

    window=make_unique<Window>(*this, wd);
    actors=make_unique<ActorManager>();
    inputSystem=make_unique<SDL_InputSystem>();

    auto cameraMan=make_shared<Actor>(*this);
    actors->addActor(cameraMan);
    cameraMan->transform.position=-5.0f*Vector3::UnitZ;

    auto camera=make_shared<CameraComponent>(*cameraMan);
    cameraMan->addComponent(camera);
    auto cmc=make_shared<CameraMoveComponent>(*cameraMan);
    cameraMan->addComponent(cmc);
    inputSystem->subscribe(cmc);

    camera->setScreenSize(wd.size.x, wd.size.y);
    window->setMainCamera(camera);

    auto lightSource=make_shared<Actor>(*this);
    actors->addActor(lightSource);

    auto dirLight=make_shared<LightComponent>(*lightSource);
    auto pointLight=make_shared<LightComponent>(*lightSource);
    auto spotLight=make_shared<LightComponent>(*lightSource);
    lightSource->addComponent(dirLight);
    lightSource->addComponent(pointLight);
    lightSource->addComponent(spotLight);

    window->addLight(dirLight);
    window->addLight(pointLight);
    window->addLight(spotLight);

    auto simpleObject=make_shared<Actor>(*this);
    actors->addActor(simpleObject);

    // auto simpleMesh=make_shared<CubeMeshComponent>(
    //     *simpleObject, window->renderer->device,
    //     "assets/crate.png"
    // );
    // auto simpleMesh=make_shared<GridMeshComponent>(
    //     *simpleObject, window->renderer->device,
    //     5, 3,
    //     "assets/crate.png"
    // );
    // auto simpleMesh=make_shared<CylinderMeshComponent>(
    //     *simpleObject, window->renderer->device,
    //     20, 5,
    //     "assets/crate.png"
    // );
    auto simpleMesh=make_shared<SphereMeshComponent>(
        *simpleObject, window->renderer->device,
        40, 15,
        "assets/ojwD8.jpg"
    );
    simpleObject->addComponent(simpleMesh);

    window->addMesh(simpleMesh);
}

Core::~Core(){
    SDL_Quit();
}

void Core::run(){
    println("Start Running...");

    isRunning=true;

    while(isRunning){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch(event.type){
            case SDL_QUIT:
                isRunning=false;
                break;
            case SDL_WINDOWEVENT:
                window->process(event.window);
                isRunning = event.window.event!=SDL_WINDOWEVENT_CLOSE;
                break;
            case SDL_KEYDOWN:
                // [[unlikely]] if(event.key.repeat==0)
                //     println("SDL_KEYDOWN");
                inputSystem->notify(EventInput{
                    .btn{
                        .keyType=event.key.keysym.sym,
                        .keydown=true,
                        .repeat=event.key.repeat!=0
                    }
                });
                break;
            case SDL_KEYUP:
                // [[unlikely]] if(event.key.repeat==0)
                //     println("SDL_KEYUP");
                inputSystem->notifyReleased(EventInput{
                    .btn{
                        .keyType=event.key.keysym.sym,
                        .keydown=false,
                        .repeat=event.key.repeat!=0
                    }
                });
                break;
            case SDL_MOUSEMOTION:
                [[fallthrough]];
            case SDL_MOUSEBUTTONDOWN:
                [[fallthrough]];
            case SDL_MOUSEBUTTONUP:
                break;
            }
        }

        if(window->isMinimized()){
            SDL_Delay(10);
        }
        else{
            window->update();

            actors->update(window->deltaTime);
        }
    }
}
