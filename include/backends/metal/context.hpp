#ifndef MODERNBOY_METAL_CONTEXT_HPP
#define MODERNBOY_METAL_CONTEXT_HPP

#include <SDL3/SDL_metal.h>
#include "render/render_command.hpp"
#include "render/render_context.hpp"
#include "resource_handle.hpp"
#include "resource_manager.hpp"
#include "mesh.hpp"
#include "shader.hpp"

namespace ModernBoy::Metal
{
    using MeshManager = ResourceManager<Metal::Mesh>;
    using MeshHandle = ResourceHandle<Metal::Mesh>;
    using ShaderManager = ResourceManager<Metal::Shader>;
    using ShaderHandle = ResourceHandle<Metal::Shader>;

    struct RenderContext{
        using Window = SDL_Window;
        using Mesh = Metal::Mesh;
        using Shader = Metal::Shader;

        SDL_MetalView view;
        NativePtr metalLayer;
        NativePtr _renderContext;
        TransformManager& transformManager;
        MeshManager& meshManager;
        ShaderManager& shaderManager;

        RenderContext(SDL_Window* in_window, TransformManager& transformManager,
            MeshManager& in_meshManager, ShaderManager& in_shaderManager);
        ~RenderContext();

        void operator()(const FrameStartCommand<Shader>&);
        void operator()(const DrawCommand<Mesh>&);
        void operator()(const FrameEndCommand&);
    };
    // static_assert(ModernBoy::RenderContext<Metal::RenderContext>);

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

#ifdef __cplusplus
}
#endif

}

#endif // MODERNBOY_DX11_CONTEXT_HPP