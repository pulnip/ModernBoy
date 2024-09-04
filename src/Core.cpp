#include <print>
#include <SDL2/SDL.h>
#include <imgui_impl_sdl2.h>
#include "Actor.hpp"
#include "ActorFactory.hpp"
#include "ActorManager.hpp"
#include "Core.hpp"
#include "helper.hpp"
#include "LevelLoader.hpp"
#include "SDL_InputSystem.hpp"
#include "Window.hpp"

using namespace std;
using namespace ModernBoy;

Core::Core(){
    using enum ActorType;
    println("start core ctor...");

    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
    constexpr auto SDL_FLAGS = SDL_INIT_VIDEO|SDL_INIT_EVENTS;
    SDL_throwIf(SDL_Init(SDL_FLAGS));

    LevelLoader levelLoader("settings/glob.json");
    auto wd=levelLoader.getWindowDesc();

    window=make_unique<Window>(*this, wd);
    actors=make_unique<ActorManager>();
    inputSystem=make_unique<SDL_InputSystem>();

    auto cameraMan=ActorFactory::make(CAMERA, *this);
    auto lightSource=ActorFactory::make(LIGHT, *this);
    auto meshObject=ActorFactory::make(TEXTURE_SPHERE, *this);

    actors->addActor(cameraMan);
    actors->addActor(lightSource);
    actors->addActor(meshObject);
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
