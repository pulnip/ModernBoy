#define SDL_MAIN_USE_CALLBACKS 1

#include <chrono>
#include <print>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui_impl_sdl3.h>
#include "engine.hpp"

using namespace ModernBoy;

#if defined(USE_DIRECTX)
using namespace ModernBoy::DX11;
#elif defined(USE_METAL)
using namespace ModernBoy::Metal;
#elif defined(USE_OPENGL)
using namespace ModernBoy::OpenGL;
#endif

Engine& engine(){
    static Engine engine = createEngine();

    return engine;
}

SDL_AppResult SDL_AppInit([[maybe_unused]] void** appState,
    [[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    auto& e = engine();
    e.start();

    *appState = &e;

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

static SDL_AppResult _handle_key_event([[maybe_unused]] void* ctx,
    SDL_Scancode key_code)
{
    switch(key_code){
    /* Quit. */
    case SDL_SCANCODE_ESCAPE:
        return SDL_APP_SUCCESS;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent([[maybe_unused]] void* appState,
    SDL_Event* event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
    switch(event->type){
    case SDL_EVENT_QUIT:
        SDL_Log("SDL_EVENT_QUIT");
        /* end the program, reporting success to the OS. */
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        return _handle_key_event(nullptr, event->key.scancode);        
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

SDL_AppResult SDL_AppIterate(void* appState){
    static_cast<Engine*>(appState)->update();

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void SDL_AppQuit(void* appState, [[maybe_unused]] SDL_AppResult result){
    static_cast<Engine*>(appState)->shutdown();
}
