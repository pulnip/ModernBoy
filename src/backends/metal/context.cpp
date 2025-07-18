#include <cassert>
#include "app_state.hpp"
#include "render/command.hpp"
#include "backends/metal/context.hpp"
#include "backends/metal/mesh.hpp"

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

using namespace ModernBoy;
using namespace ModernBoy::Metal;

RenderContext::RenderContext(SDL_Window* window)
:view(SDL_Metal_CreateView(window)),
metalLayer(SDL_Metal_GetLayer(view)),
_renderContext(createRenderContext(metalLayer)){}

RenderContext::~RenderContext(){
    destroyRenderContext(_renderContext);
    SDL_Metal_DestroyView(view);
}

void RenderContext::onFrameStart(Vec4 clearColor){
    assert(_renderContext != nullptr);
    RenderContext_frameStart(_renderContext,
        clearColor.r, clearColor.g, clearColor.b,
        clearColor.a);
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
void RenderContext::drawMesh(Vec3 position,
    Vec4 rotation, Vec3 scale,
    NativePtr mesh
){
    assert(_renderContext != nullptr);
    RenderContext_draw_(_renderContext,
        position.x, position.y, position.z,
        rotation.x, rotation.y, rotation.z,
        scale.x, scale.y, scale.z,
        mesh);
}
void RenderContext::onFrameEnd(){
    assert(_renderContext != nullptr);
    RenderContext_frameEnd(_renderContext);
}

NativePtr RenderContext::getRenderPassDesc(){
    return RenderContext_getRenderPassDesc(_renderContext);
}
NativePtr RenderContext::getDevice(){
    return RenderContext_getDevice(_renderContext);
}
NativePtr RenderContext::getCommandBuffer(){
    return RenderContext_getCommandBuffer(_renderContext);
}
NativePtr RenderContext::getRenderEncoder(){
    return RenderContext_getRenderEncoder(_renderContext);
}