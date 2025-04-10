#include <stdexcept>
#include <SDL3/SDL_log.h>
#include "backends/metal/bridge.hpp"
#include "backends/metal/context.hpp"
#include <imgui.h>
// #include <imgui_impl_sdl3.h>

using namespace ModernBoy::Metal;

RenderContext::RenderContext([[maybe_unused]] SDL_Window* in_window,
    MeshManager& in_meshManager)
:meshManager(in_meshManager){}
RenderContext::~RenderContext(){}

void RenderContext::operator()([[maybe_unused]] const StartCommand& cmd){
    renderStart();
}

void RenderContext::operator()([[maybe_unused]] const DrawCommand<Mesh>& cmd){
    draw();
}

void RenderContext::operator()([[maybe_unused]] const ClearCommand& cmd){
    renderEnd();
}
