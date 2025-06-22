#ifndef MODERNBOY_METAL_CONTEXT_HPP
#define MODERNBOY_METAL_CONTEXT_HPP

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_metal.h>
#include "fwd.hpp"

namespace ModernBoy::Metal
{
    struct RenderContext{
        SDL_MetalView view;
        NativePtr metalLayer;
        NativePtr _renderContext;

        RenderContext(SDL_Window* in_window, AppState& app);
        ~RenderContext();

        void operator()(const FrameStartCommand<Shader>&);
        void operator()(const DrawCommand<Mesh>&);
        void operator()(const FrameEndCommand&);

    private:
        AppState& app;
        ObserverID fov_id = -1;
    };

#ifdef __cplusplus
extern "C"{
#endif

    extern void* createRenderContext(const void* layerPtr);
    extern void destroyRenderContext(void* ctx);

    extern void RenderContext_frameStart(
        void* _nativeContext,
        double r, double g, double b, double a,
        void* shaderPtr
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

    extern void RenderContext_setView(
        void* _nativeContext,
        float px, float py, float pz,
        float rx, float ry, float rz, float w
    );

    extern void RenderContext_setfov(
        void* _nativeContext, float fov
    );

#ifdef __cplusplus
}
#endif

}

#endif // MODERNBOY_METAL_CONTEXT_HPP