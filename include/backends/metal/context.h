#ifndef MODERNBOY_METAL_CONTEXT_H
#define MODERNBOY_METAL_CONTEXT_H

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
        void* meshPtr, float alpha
    );
    extern void RenderContext_draw_(
        void* _nativeContext,
        float px, float py, float pz,
        float rx, float ry, float rz,
        float sx, float sy, float sz,
        void* meshPtr, float alpha
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

#endif // MODERNBOY_METAL_CONTEXT_H