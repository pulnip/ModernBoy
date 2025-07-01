#define SDL_MAIN_USE_CALLBACKS 1

#include <chrono>
#include <print>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui_impl_sdl3.h>
#include "app_state.hpp"
#if defined(USE_DIRECTX)
#include "backends/dx11/mesh.hpp"
#elif defined(USE_OPENGL)
#include <glad/glad.h>
#endif

using namespace ModernBoy;

#if defined(USE_DIRECTX)
using namespace ModernBoy::DX11;
#elif defined(USE_METAL)
using namespace ModernBoy::Metal;
#elif defined(USE_OPENGL)
using namespace ModernBoy::OpenGL;
#endif

static SDL_AppResult _handle_key_event(void* ctx, SDL_Scancode key_code);

SDL_AppResult SDL_AppInit(void** appState,
    [[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    if(!SDL_SetAppMetadata("ModernBoy", "1.0", "com.example.game0")){
        return SDL_APP_FAILURE;
    }

    if(!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Init Window
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
        return SDL_APP_FAILURE;
    }

    *appState = new AppState(window);
    if(!*appState) return SDL_APP_FAILURE;
    AppState& app = *static_cast<AppState*>(*appState);

    // Temporal codes...
#if defined(USE_DIRECTX)
    DX11::Mesh rect;
    if(!DX11::makeTetra(as->renderer.context.device, rect)){
        SDL_Log("Failed to create rect mesh");
        return SDL_APP_FAILURE;
    }

    MeshHandle rectHandle = as->meshManager.create(std::move(rect));
    as->taskManager.create(MeshComponent{
        .owner = 0,
        .resourceHandle = rectHandle
    });

    DX11::DefaultShader shader(as->renderer.context.device);
    as->shaderManager.create(std::move(shader));
#elif defined(USE_OPENGL)
    // TODO
#endif
    app.assetLoader.loadScripts("asset/action.toml");
    app.assetLoader.loadActors("asset/actor.toml");

    app.lastTicks = SDL_GetTicks();

    return SDL_APP_CONTINUE;  /* carry on with the program! */
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

static SDL_AppResult _handle_key_event([[maybe_unused]] void* ctx,
    SDL_Scancode key_code)
{
    switch(key_code){
    /* Quit. */
    case SDL_SCANCODE_ESCAPE:
    case SDL_SCANCODE_Q:
        return SDL_APP_SUCCESS;
    case SDL_SCANCODE_R:
        break;
    case SDL_SCANCODE_RIGHT:
        break;
    case SDL_SCANCODE_UP:
        break;
    case SDL_SCANCODE_LEFT:
        break;
    case SDL_SCANCODE_DOWN:
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState){
    AppState& app = *static_cast<AppState*>(appState);

    Uint64 now = SDL_GetTicks();
    if((now - app.lastTicks) == 0)
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    now = SDL_GetTicks();
    Uint64 deltaTime = now - app.lastTicks;
    app.lastTicks = now;
    app.deltaTime = deltaTime;

    Game::Context& world = app.world;
    app.inputSystem.update(deltaTime);

    world.update(deltaTime);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void SDL_AppQuit(void* appState, [[maybe_unused]] SDL_AppResult result){
    if(appState != NULL){
        AppState* as = (AppState*)appState;
        SDL_Window* window = as->window;

        delete as;
        /* SDL will clean up the window/renderer for us. */
        SDL_DestroyWindow(window);
    }
}
