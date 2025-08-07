#ifndef MODERNBOY_METAL_CONTEXT_H
#define MODERNBOY_METAL_CONTEXT_H

#include "common/type.hpp"

#ifdef __cplusplus
extern "C"{
#endif

    extern void* createRenderContext(const void* layerPtr,
        const char* shaderLibPath
    );
    extern void destroyRenderContext(void* ctx);

    extern void RenderContext_frameStart(
        void* nativeContext,
        double r, double g, double b, double a
    );
    extern void RenderContext_setView(
        void* nativeContext,
        float px, float py, float pz, float fov,
        float rx, float ry, float rz, float w
    );
    extern void RenderContext_setShader(
        void* nativeContext,
        void* shaderPtr
    );
    extern void RenderContext_setTexture(
        void* nativeContext,
        void* texture
    );
    extern void RenderContext_draw(
        void* nativeContext,
        float px, float py, float pz,
        float rx, float ry, float rz, float w,
        float sx, float sy, float sz,
        void* meshPtr, float alpha, int id,
        bool useUV = true,
        float r=0, float g=0, float b=0, float a=1
    );
    extern void RenderContext_frameEnd(
        void* nativeContext,
        const ModernBoy::Line* lines, int count
    );
    extern void* RenderContext_getDevice(
        void* nativeContext
    );
    extern void* RenderContext_getRenderPassDesc(
        void* nativeContext
    );
    extern void* RenderContext_getCommandBuffer(
        void* nativeContext
    );
    extern void* RenderContext_getRenderEncoder(
        void* nativeContext
    );

    extern int RenderContext_getPickedID(
        void* nativeContext, int x, int y
    );

#ifdef __cplusplus
}
#endif

#endif // MODERNBOY_METAL_CONTEXT_H