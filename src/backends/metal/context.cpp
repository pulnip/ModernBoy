#include <stdexcept>
#include <SDL3/SDL_log.h>
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include "backends/metal/bridge.hpp"
#include "backends/metal/context.hpp"
#include <imgui.h>
// #include <imgui_impl_sdl3.h>

using namespace ModernBoy::Metal;

RenderContext::RenderContext(SDL_Window* in_window, MeshManager& in_meshManager,
    ShaderManager& in_shaderManager)
:view(SDL_Metal_CreateView(in_window)),
metalLayer(SDL_Metal_GetLayer(view)),
meshManager(in_meshManager), shaderManager(in_shaderManager){}
RenderContext::~RenderContext(){}

void RenderContext::operator()([[maybe_unused]] const FrameStartCommand<Shader>& cmd){
    renderStart();
}

void RenderContext::operator()([[maybe_unused]] const DrawCommand<Mesh>& cmd){
    draw();
}

void RenderContext::operator()([[maybe_unused]] const FrameEndCommand& cmd){
    renderEnd();
}
