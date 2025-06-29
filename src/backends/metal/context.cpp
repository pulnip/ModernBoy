#include <cassert>
#include <imgui_impl_sdl3.h>
#define IMGUI_IMPL_METAL_CPP
#include <imgui_impl_metal.h>
#include "app_state.hpp"
#include "render/command.hpp"
#include "backends/metal/context.hpp"
#include "backends/metal/mesh.hpp"
#include "render/gui.hpp"

#ifdef __cplusplus
extern "C"{
#endif

    extern void* createRenderContext(const void* layerPtr);
    extern void destroyRenderContext(void* ctx);

    extern void RenderContext_frameStart(
        void* _nativeContext,
        double r, double g, double b, double a
    );
    extern void RenderContext_setView(
        void* _nativeContext,
        float px, float py, float pz, float fov,
        float rx, float ry, float rz, float w
    );
    extern void RenderContext_setShader(
        void* _nativeContext,
        void* shaderPtr
    );
    extern void RenderContext_setTexture(
        void* _nativeContext,
        void* texture
    );
    extern void RenderContext_draw(
        void* _nativeContext,
        float px, float py, float pz,
        float rx, float ry, float rz, float w,
        float sx, float sy, float sz,
        void* meshPtr
    );
    extern void RenderContext_draw_(
        void* _nativeContext,
        float px, float py, float pz,
        float rx, float ry, float rz,
        float sx, float sy, float sz,
        void* meshPtr
    );
    extern void RenderContext_frameEnd(
        void* _nativeContext
    );
    extern void* RenderContext_getDevice(
        void* _nativeContext
    );
    extern void* RenderContext_getRenderPassDesc(
        void* _nativeContext
    );
    extern void* RenderContext_getCommandBuffer(
        void* _nativeContext
    );
    extern void* RenderContext_getRenderEncoder(
        void* _nativeContext
    );

#ifdef __cplusplus
}
#endif

using namespace ModernBoy::Render;
using namespace ModernBoy::Metal;

RenderContext::RenderContext(SDL_Window* window)
:view(SDL_Metal_CreateView(window)),
metalLayer(SDL_Metal_GetLayer(view)){}

RenderContext::~RenderContext(){
    SDL_Metal_DestroyView(view);
    destroyRenderContext(_renderContext);
}

void RenderContext::initialize(SDL_Window* window){
    _renderContext = createRenderContext(metalLayer);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForMetal(window);
    auto device = static_cast<MTL::Device*>(
        RenderContext_getDevice(_renderContext));
    ImGui_ImplMetal_Init(device);
}

void RenderContext::onFrameStart(Vec4 clearColor){
    assert(_renderContext != nullptr);
    RenderContext_frameStart(_renderContext,
        clearColor.r, clearColor.g, clearColor.b,
        clearColor.a);

    auto renderPassDesc = static_cast<MTL::RenderPassDescriptor*>(
        RenderContext_getRenderPassDesc(_renderContext));
    // Start the Dear ImGui frame
    ImGui_ImplMetal_NewFrame(renderPassDesc);
    ImGui_ImplSDL3_NewFrame();
}
void RenderContext::setView(float fov, Vec3 viewPos,
    Vec4 viewQuat
){
    RenderContext_setView(_renderContext,
        viewPos.x, viewPos.y, viewPos.z, fov,
        viewQuat.x, viewQuat.y, viewQuat.z, viewQuat.w
    );
}
void RenderContext::setShader(NativePtr shader){
    assert(_renderContext != nullptr);

    RenderContext_setShader(_renderContext, shader);
}
void RenderContext::setTexture(NativePtr texture){
    assert(_renderContext != nullptr);

    RenderContext_setTexture(_renderContext, texture);
}
void RenderContext::drawMesh(Transform transform,
    NativePtr mesh
){
    const auto& position = transform.position;
    const auto& rotation = transform.rotation;
    const auto& scale = transform.scale;

    assert(_renderContext != nullptr);
    RenderContext_draw_(_renderContext,
        position.x, position.y, position.z,
        rotation.x, rotation.y, rotation.z,
        scale.x, scale.y, scale.z,
        mesh);
}
void RenderContext::onFrameEnd(ImDrawData* uiDrawData){
    assert(_renderContext != nullptr);
    auto commandBuffer = static_cast<MTL::CommandBuffer*>(
        RenderContext_getCommandBuffer(_renderContext));
    auto renderEncoder = static_cast<MTL::RenderCommandEncoder*>(
        RenderContext_getRenderEncoder(_renderContext));

    ImGui_ImplMetal_RenderDrawData(uiDrawData,
        commandBuffer, renderEncoder
    );
    assert(commandBuffer != nullptr);
    assert(renderEncoder != nullptr);
    RenderContext_frameEnd(_renderContext);
}
