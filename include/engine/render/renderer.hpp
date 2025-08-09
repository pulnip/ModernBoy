#ifndef MODERNBOY_RENDER_SYSTEM_HPP
#define MODERNBOY_RENDER_SYSTEM_HPP

#include <atomic>
#include <chrono>
#include <map>
#include <unordered_set>
#include <SDL3/SDL_video.h>
#include "engine/fwd.hpp"
#include "engine/task.hpp"
#include "core/alias.hpp"
#include "engine/interface.hpp"
#include "core/thread/lock_free_queue.hpp"
#include "core/thread/generator.hpp"
#include "engine/render/command.hpp"
#include "engine/service/debug_draw_service.hpp"
#if defined(USE_DIRECTX)
#include "engine/render/dx11/context.hpp"
#elif defined(USE_METAL)
#include <SDL3/SDL_metal.h>
#include "engine/render/metal/context.h"
#include "engine/render/metal/mesh.hpp"
#elif defined(USE_OPENGL)
#include "engine/render/opengl/context.hpp"
#endif

namespace ModernBoy::Render
{
    class Renderer{
    public:
        Renderer(SDL_Window*, MeshManager&,
            TextureManager&, ShaderManager&,
            World&, Service::DebugDrawService&);
        ~Renderer();

        TaskTime expectedExecTime();
        void onFrameStart();
        void update(DeltaTime);
        // Generator<void> update(DeltaTime);
        void onFrameEnd();

        EntityID queryWindowPos(int x, int y);

#if defined(USE_DIRECTX)
        NativePtr getDevice();
        NativePtr getContext();
#elif defined(USE_METAL)
        NativePtr getRenderPassDesc();
        NativePtr getDevice();
        NativePtr getCommandBuffer();
        NativePtr getRenderEncoder();
#endif

        const TaskPolicy policy{
            .effective_window_size = 10,
            .min_interval = std::chrono::microseconds(1000/60),
            .patience = std::chrono::milliseconds(0)
        };

    private:
        void updateEMA(TaskTime);

        void setView(const ViewTask& task);
        void setShader(ShaderHandle handle);
        void setTexture(TextureHandle handle);
        void drawMesh(const Vec3& position,
            const Vec4& rotation, const Vec3& scale,
            MeshHandle handle, float alpha, int id);
        void drawMesh(const Vec3& position,
            const Vec4& rotation, const Vec3& scale,
            const Mesh& mesh, float alpha, int id,
            bool useUV = true,
            const Vec4& color = Vec4{.r=1, .g=0, .b=0, .a=1});

    public:
#if defined(USE_DIRECTX)
        RenderContext context;
#elif defined(USE_METAL)
        SDL_MetalView view;
        NativePtr metalLayer;
        NativePtr context;
#endif
    private:
        MeshManager& meshManager;
        TextureManager& textureManager;
        ShaderManager& shaderManager;
        World& world;

        TaskTime ema;

        Service::DebugDrawService& ddService;
        Mesh sphereMesh;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_SYSTEM_HPP