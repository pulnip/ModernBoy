#include <chrono>
#include <numbers>
#include <stdexcept>
#include <SDL3/SDL_log.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include "app_state.hpp"
#include "render/command.hpp"
#include "backends/metal/context.hpp"
#define IMGUI_IMPL_METAL_CPP
#include <imgui_impl_metal.h>

using namespace std::chrono;
using namespace ModernBoy::Metal;

RenderContext::RenderContext(SDL_Window* in_window, AppState& app)
:view(SDL_Metal_CreateView(in_window)),
metalLayer(SDL_Metal_GetLayer(view)),
app(app){
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
    io.DisplaySize = ImVec2(w, h);
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    // ImGui::StyleColorsLight();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForMetal(in_window);
    auto device = static_cast<MTL::Device*>(
        RenderContext_getDevice(_renderContext));
    ImGui_ImplMetal_Init(device);

    fov_id = app.gui.subscribefieldOfView(
        [rctx=_renderContext](float fov){
            RenderContext_setfov(rctx, fov);
        }
    );
}
RenderContext::~RenderContext(){
    app.gui.unsubscribefieldOfView(fov_id);

    SDL_Metal_DestroyView(view);
    destroyRenderContext(_renderContext);
}

void RenderContext::operator()(const FrameStartCommand_& cmd){
    auto shaderPtr = app.get<Shader>(cmd.shaderHandle).shaderPtr;
    const auto& cameraTransform = app.get<Transform>(cmd.cameraTransformHandle);
    const auto& viewPos = cameraTransform.position;
    const auto& viewQuat = cameraTransform.rotation;
    const auto& camera = app.get<Camera>(cmd.cameraHandle);

    RenderContext_setView(_renderContext,
        viewPos.x, viewPos.y, viewPos.z,
        viewQuat.x, viewQuat.y, viewQuat.z, viewQuat.w
    );

    RenderContext_frameStart(_renderContext,
        0.0, 0.0, 0.0, 0.5, shaderPtr);

    auto renderPassDesc = static_cast<MTL::RenderPassDescriptor*>(
        RenderContext_getRenderPassDesc(_renderContext));
    // Start the Dear ImGui frame
    ImGui_ImplMetal_NewFrame(renderPassDesc);
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();
    // ImGui::ShowDemoWindow(); // Show demo window! :)
    app.gui.update();
}

void RenderContext::operator()(const DrawCommand_& cmd){
    Transform transform = app.get<Transform>(cmd.transformHandle);
    float *p=transform.pos, *r=transform.rot, *s=transform.scl;
    
    MeshPtr meshPtr = app.get<Mesh>(cmd.meshHandle).meshPtr;

    auto now = steady_clock::now().time_since_epoch();
    float seconds = duration<float>(now).count();
    float ry = fmodf(seconds * (float)(std::numbers::pi/2.0), (float)(std::numbers::pi * 2.0));

    RenderContext_draw_(_renderContext, p[0], p[1], p[2],
        r[0], ry, r[2], s[0], s[1], s[2], meshPtr);
}

void RenderContext::operator()([[maybe_unused]] const FrameEndCommand_& cmd){
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
