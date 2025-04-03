#define SDL_MAIN_USE_CALLBACKS 1

#include <print>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include "app_state.hpp"
#ifdef USE_DIRECTX
#include <imgui_impl_dx11.h>
#include "backends/dx11_adaptor.hpp"
#elif defined(USE_OPENGL)
#include <glad/glad.h>
#include <imgui_impl_opengl3.h>
#endif

constexpr auto STEP_RATE_IN_MILLISECONDS = 1000;

static SDL_AppResult _ImGui_Init(AppState* appState){
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    int w, h;
    if(!SDL_GetWindowSize(appState->window, &w, &h)){
        SDL_Log("SDL_GetWindowSize Failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImVec2(w, h);

    // Setup Platform/Renderer backends
    #ifdef USE_DIRECTX
    ImGui_ImplSDL3_InitForD3D(appState->window);
    ImGui_ImplDX11_Init(appState->renderState->device.Get(),
        appState->renderState->context.Get());
#elif defined(USE_OPENGL)
    // TODO
    ImGui_ImplSDL3_InitForOpenGL(appState->window, );
    ImGui_ImplOpenGL3_Init();
#endif

    return SDL_APP_CONTINUE;
}

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

    AppState* as = (AppState*)SDL_calloc(1, sizeof(AppState));
    if(!as) return SDL_APP_FAILURE;

    // Init Window
    SDL_WindowFlags flags = SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS;

#ifdef USE_DIRECTX
    as->renderState = (DX11RenderState*)SDL_calloc(1, sizeof(DX11RenderState));
    if(!as->renderState) return SDL_APP_FAILURE;
    
    *appState = as;
#elif defined(USE_OPENGL)
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    flags |= SDL_WINDOW_OPENGL;
#endif

    as->window = SDL_CreateWindow("examples/demo/game0", 800, 600, flags);
    if(as->window == nullptr){
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Init Renderer
#ifdef USE_DIRECTX
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(as->window),
        SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    DX11RenderState_Init(hwnd, as->renderState);
#elif defined(USE_OPENGL)
    // TODO
#endif

    // Init ImGui
    _ImGui_Init(as);

    as->last_step = SDL_GetTicks();
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

SDL_AppResult SDL_AppEvent([[maybe_unused]] void* appState,
    SDL_Event* event)
{
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
    AppState& as = *(AppState*)appState;
    GameContext& ctx = as.game_ctx;

    const Uint64 now = SDL_GetTicks();

    while((now - as.last_step) >= STEP_RATE_IN_MILLISECONDS){
        ctx.shown = !ctx.shown;
        as.last_step += STEP_RATE_IN_MILLISECONDS;
    }

    // Start the Dear ImGui frame
#ifdef USE_DIRECTX
    ImGui_ImplDX11_NewFrame();
#elif defined(USE_OPENGL)
    ImGui_ImplOpenGL3_NewFrame();
#endif
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow(); // Show demo window! :)

    // Rendering
#ifdef USE_DIRECTX
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
    DX11RenderState& rs = *(as.renderState);
    
    rs.context->RSSetViewports(1, &rs.viewport);
    rs.context->OMSetRenderTargets(1, rs.rtv.GetAddressOf(), rs.dsv.Get());
    rs.context->ClearRenderTargetView(rs.rtv.Get(), clearColor);
    rs.context->OMSetDepthStencilState(rs.dss.Get(), 0);
    rs.context->ClearDepthStencilView(rs.dsv.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
    );

    // set the shader objects
    rs.context->VSSetShader(rs.shaderState.vs.Get(), 0, 0);
    rs.context->PSSetShader(rs.shaderState.ps.Get(), 0, 0);

    if(ctx.shown){
        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        rs.context->IASetInputLayout(rs.shaderState.il.Get());
        rs.context->IASetVertexBuffers(0, 1,
            rs.shaderState.vertexBuffer.GetAddressOf(), &stride, &offset);

        rs.context->IASetIndexBuffer(rs.shaderState.indexBuffer.Get(),
            DXGI_FORMAT_R32_UINT, 0);

        rs.context->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );
        rs.context->DrawIndexed(6, 0, 0);
    }
#elif defined(USE_OPENGL)
    // TODO
#endif

    ImGui::Render();

#ifdef USE_DIRECTX
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    rs.swapChain->Present(1, 0);
#elif defined(USE_OPENGL)
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(as.window);
#endif

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void SDL_AppQuit(void* appState, [[maybe_unused]] SDL_AppResult result){
#ifdef USE_DIRECTX
    ImGui_ImplDX11_Shutdown();
#elif defined(USE_OPENGL)
    ImGui_ImplOpenGL3_Shutdown();
#endif
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    if(appState != NULL){
        AppState* as = (AppState*)appState;
        /* SDL will clean up the window/renderer for us. */
        SDL_DestroyWindow(as->window);
#ifdef USE_DIRECTX
        SDL_free(as->renderState);
#elif defined(USE_OPENGL)
    // TODO
#endif
        SDL_free(as);
    }
}
