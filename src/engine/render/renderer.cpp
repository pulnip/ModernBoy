#include <algorithm>
#if defined(USE_METAL)
#include <SDL3/SDL_metal.h>
#endif
#include "engine/log/log.hpp"
#include "engine/resource_manager.hpp"
#include "engine/render/renderer.hpp"
#include "engine/engine.hpp"
#include "engine/interface/draw_service.hpp"
#include "engine/interface/view_service.hpp"

using namespace std::chrono_literals;
using namespace ModernBoy;
using namespace ModernBoy::Render;
using namespace ModernBoy::Interface;

Renderer::Renderer(SDL_Window* window,
    SubmeshManager&submeshManager, MaterialManager& materialManager,
    ShaderManager& shaderManager, World& world,
    ViewService& viewSrv, DrawService& drawSrv)
#if defined(USE_DIRECTX)
:context(window),
#elif defined(USE_METAL)
:view(SDL_Metal_CreateView(window)),
metalLayer(SDL_Metal_GetLayer(view)),
context(createRenderContext(metalLayer,
    "./asset/shader/ModernBoy.metallib")),
#endif
submeshManager(submeshManager), materialManager(materialManager),
shaderManager(shaderManager), world(world), ema(0ms),
viewService(viewSrv), drawService(drawSrv){}

Renderer::~Renderer(){
#if defined(USE_METAL)
    destroyRenderContext(context);
    SDL_Metal_DestroyView(view);
#endif
}

void Renderer::update(DeltaTime){
    auto started = std::chrono::steady_clock::now();
    auto debugSpheres = drawService.drainSpheres();

    // auto meshObjects = drawService.drainMeshObjects();
    auto meshDrawCalls = drawService.drainDrawCalls();
    auto cameraObjects = viewService.drainCameraObjects();

    for(const auto& cameraObj: cameraObjects){
        setView(cameraObj);

        auto shaderHandle = invalidHandle();
        auto materialHandle = invalidHandle();
        for(const auto& drawCall: meshDrawCalls){
            if(drawCall.shaderHandle != shaderHandle){
                shaderHandle = drawCall.shaderHandle;
                setShader(shaderHandle);
            }
            if(drawCall.materialHandle != materialHandle){
                materialHandle = drawCall.materialHandle;
                setMaterial(materialHandle);
            }

            drawSubmesh(drawCall.position, drawCall.rotation,
                drawCall.scale, drawCall.submeshHandle, drawCall.alpha,
                static_cast<int>(drawCall.entity));
        }

        for(const auto& [pos, radius, color]: debugSpheres){
            drawSubmesh(pos, unitQuat(),
                Vec3{.x=radius, .y=radius, .z=radius},
                sphereMesh, 0.0f, 0,
                false, color);
        }
    }

    auto elapsed = std::chrono::steady_clock::now() - started;
    updateEMA(std::chrono::duration_cast<TaskTime>(elapsed));
}
void Renderer::onFrameStart(){
    RenderTrace("Frame Start");
#if defined(USE_DIRECTX)
    context.onFrameStart(
        {.r=0.0f, .g=0.0f, .b=0.0f, .a=0.5f});
#elif defined(USE_METAL)
    RenderContext_frameStart(context,
        0.0f, 0.0f, 0.0f, 0.5f);
#endif
}

void Renderer::updateEMA(TaskTime elapsed){
    auto alpha = 1.0 / policy.effective_window_size;
    long long blended = (1-alpha)*ema.count() + alpha*elapsed.count();
    ema = TaskTime(blended);
}

void Renderer::setView(const CameraObject& camObj){
    const auto& viewPos = camObj.position;
    const auto& viewQuat = camObj.rotation;
    RenderTrace("Set View, pos: {}, {}, {}",
        viewPos.x, viewPos.y, viewPos.z);

#if defined(USE_DIRECTX)
    context.setView(camObj.fov, viewPos, viewQuat);
#elif defined(USE_METAL)
    RenderContext_setView(context ,
        viewPos.x, viewPos.y, viewPos.z, camObj.fov,
        viewQuat.x, viewQuat.y, viewQuat.z, viewQuat.w
    );
#endif
}
void Renderer::setShader(ShaderHandle handle){
    RenderTrace("Set Shader, index: {}", handle.index);
    const auto& shader = shaderManager.get(handle);
#if defined(USE_DIRECTX)
    context.setShader(shader.shaderPtr);
#elif defined(USE_METAL)
    RenderContext_setShader(context, shader.shaderPtr);
#endif
}
void Renderer::setMaterial(MaterialHandle handle){
    RenderTrace("Set Material, Index: {}", handle.index);
    const auto& material = materialManager.get(handle);
    RenderTrace("  Material Ptr: {}", material.material);
#if defined(USE_METAL)
    RenderContext_setUnlitMaterial(context, material.material);
#endif
}
void Renderer::drawSubmesh(const Vec3& position,
    const Vec4& rotation, const Vec3& scale,
    SubmeshHandle handle, float alpha, int id
){
    RenderTrace("index: {}", handle.index);
    const auto& submesh = submeshManager.get(handle);

#if defined(USE_DIRECTX)
    context.drawMesh(position, rotation, scale, partPtr, alpha);
#elif defined(USE_METAL)
    RenderContext_draw(context,
        position.x, position.y, position.z,
        rotation.x, rotation.y, rotation.z, rotation.w,
        scale.x, scale.y, scale.z,
        submesh.nativeMesh, alpha, id);
#endif
}
void Renderer::drawSubmesh(
    const Vec3& position, const Vec4& rotation,
    const Vec3& scale, const Submesh& mesh,
    float alpha, int id,
    bool useUV, const Vec4& color
){
#if defined(USE_DIRECTX)
    context.drawMesh(position, rotation, scale, partPtr, alpha);
#elif defined(USE_METAL)
    RenderContext_draw(context,
        position.x, position.y, position.z,
        rotation.x, rotation.y, rotation.z, rotation.w,
        scale.x, scale.y, scale.z,
        mesh.nativeMesh, alpha, id,
        useUV, color.r, color.g, color.b, color.a);
#endif
}

void Renderer::onFrameEnd(){
    RenderTrace("Frame End");
    auto debugLines = drawService.drainLines();

#if defined(USE_DIRECTX)
    context.onFrameEnd();
#elif defined(USE_METAL)
    RenderContext_frameEnd(context,
        debugLines.data(), debugLines.size());
#endif
}

EntityID Renderer::queryWindowPos(int x, int y){
    int id = RenderContext_getPickedID(context, x, y);
    return static_cast<EntityID>(id);
}

#if defined(USE_DIRECTX)
NativePtr Renderer::getDevice(){
    return context.getDevice();
}
NativePtr Renderer::getContext(){
    return context.getContext();
}
#elif defined(USE_METAL)
NativePtr Renderer::getRenderPassDesc(){
    return RenderContext_getRenderPassDesc(context);
}
NativePtr Renderer::getDevice(){
    return RenderContext_getDevice(context);
}
NativePtr Renderer::getCommandBuffer(){
    return RenderContext_getCommandBuffer(context);
}
NativePtr Renderer::getRenderEncoder(){
    return RenderContext_getRenderEncoder(context);
}
#endif
