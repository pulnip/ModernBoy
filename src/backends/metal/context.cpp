#include <stdexcept>
#include <SDL3/SDL_log.h>
#include "backends/metal/context.hpp"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#define IMGUI_IMPL_METAL_CPP
#include <imgui_impl_metal.h>

#include <cmath>
#include <chrono>
using namespace std::chrono;

using namespace ModernBoy::Metal;

RenderContext::RenderContext(SDL_Window* in_window, TransformManager& transformManager,
    MeshManager& in_meshManager, ShaderManager& in_shaderManager,
    CameraManager& cameraManager)
:view(SDL_Metal_CreateView(in_window)),
metalLayer(SDL_Metal_GetLayer(view)), transformManager(transformManager),
meshManager(in_meshManager), shaderManager(in_shaderManager),
cameraManager(cameraManager){
    NativePtr layer = SDL_Metal_GetLayer(view);
    _renderContext = createRenderContext(layer);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    int w, h;
    if(!SDL_GetWindowSize(in_window, &w, &h)){
        SDL_Log("SDL_GetWindowSize Failed: %s", SDL_GetError());
        throw 1;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImVec2(w, h);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForMetal(in_window);
    auto device = static_cast<MTL::Device*>(
        RenderContext_getDevice(_renderContext));
    ImGui_ImplMetal_Init(device);
}
RenderContext::~RenderContext(){
    SDL_Metal_DestroyView(view);
    destroyRenderContext(_renderContext);
}

void RenderContext::operator()(const FrameStartCommand<Shader>& cmd){
    ShaderPtr shaderPtr = shaderManager.get(cmd.shaderHandle)->shaderPtr;
    const auto& cameraTransform = *transformManager.get(cmd.cameraTransformHandle);
    const auto& viewPos = cameraTransform.position;
    const auto& viewQuat = cameraTransform.rotation;
    const auto& camera = *cameraManager.get(cmd.cameraHandle);

    RenderContext_setView(_renderContext,
        viewPos[0], viewPos[1], viewPos[2],
        viewQuat[0], viewQuat[1], viewQuat[2], viewQuat[3]
    );

    RenderContext_frameStart(_renderContext,
        0.0, 0.0, 0.0, 0.5, shaderPtr);

    auto renderPassDesc = static_cast<MTL::RenderPassDescriptor*>(
        RenderContext_getRenderPassDesc(_renderContext));
    // Start the Dear ImGui frame
    ImGui_ImplMetal_NewFrame(renderPassDesc);
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)
}

void RenderContext::operator()(const DrawCommand<Mesh>& cmd){
    Transform transform = *transformManager.get(cmd.transformHandle);
    float *p=transform.position, *r=transform.rotation, *s=transform.scale;
    
    MeshPtr meshPtr = meshManager.get(cmd.meshHandle)->meshPtr;

    auto now = steady_clock::now().time_since_epoch();
    float seconds = duration<float>(now).count();
    float ry = fmodf(seconds * (float)(M_PI/2.0), (float)(M_PI * 2.0));

    RenderContext_draw_(_renderContext, p[0], p[1], p[2],
        r[0], ry, r[2], s[0], s[1], s[2], meshPtr);
}

void RenderContext::operator()([[maybe_unused]] const FrameEndCommand& cmd){
    auto commandBuffer = static_cast<MTL::CommandBuffer*>(
        RenderContext_getCommandBuffer(_renderContext));
    auto renderEncoder = static_cast<MTL::RenderCommandEncoder*>(
        RenderContext_getRenderEncoder(_renderContext));
    ImGui::Render();
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(),
        commandBuffer, renderEncoder
    );

    RenderContext_frameEnd(_renderContext);
}
