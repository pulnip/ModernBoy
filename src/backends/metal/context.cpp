#include <stdexcept>
#include <SDL3/SDL_log.h>
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "backends/metal/context.hpp"
#include <imgui.h>
// #include <imgui_impl_sdl3.h>

using namespace ModernBoy::Metal;

RenderContext::RenderContext(SDL_Window* in_window, MeshManager& in_meshManager,
    ShaderManager& in_shaderManager)
:view(SDL_Metal_CreateView(in_window)),
metalLayer(SDL_Metal_GetLayer(view)),
meshManager(in_meshManager), shaderManager(in_shaderManager)
{
    NativePtr layer = SDL_Metal_GetLayer(view);
    _renderContext = createRenderContext(layer);
}
RenderContext::~RenderContext(){
    SDL_Metal_DestroyView(view);
    destroyRenderContext(_renderContext);
}

void RenderContext::operator()([[maybe_unused]] const FrameStartCommand<Shader>& cmd){
    ShaderPtr shaderPtr = shaderManager.get(cmd.getHandle())->shaderPtr;

    RenderContext_frameStart(_renderContext,
        0.0, 0.0, 0.0, 0.5, shaderPtr);
}

void RenderContext::operator()([[maybe_unused]] const DrawCommand<Mesh>& cmd){
    MeshPtr meshPtr = meshManager.get(cmd.getHandle())->meshPtr;

    RenderContext_draw(_renderContext, meshPtr);
}

void RenderContext::operator()([[maybe_unused]] const FrameEndCommand& cmd){
    RenderContext_frameEnd(_renderContext);
}
