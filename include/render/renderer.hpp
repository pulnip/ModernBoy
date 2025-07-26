#ifndef MODERNBOY_RENDER_SYSTEM_HPP
#define MODERNBOY_RENDER_SYSTEM_HPP

#include <atomic>
#include <chrono>
#include <map>
#include <unordered_set>
#include <SDL3/SDL_video.h>
#include "fwd.hpp"
#include "task.hpp"
#include "common/alias.hpp"
#include "interface.hpp"
#include "util/lock_free_queue.hpp"
#include "util/generator.hpp"
#include "render/command.hpp"
#if defined(USE_DIRECTX)
#include "backends/dx11/context.hpp"
#elif defined(USE_METAL)
#include "backends/metal/context.hpp"
#elif defined(USE_OPENGL)
#include "backends/opengl/context.hpp"
#endif

namespace ModernBoy::Render
{
    class Renderer{
    public:
        Renderer(SDL_Window*, MeshManager&,
            TextureManager&, ShaderManager&,
            World&);
        ~Renderer() = default;

        TaskTime expectedExecTime();
        void onFrameStart();
        void update(DeltaTime);
        // Generator<void> update(DeltaTime);
        void onFrameEnd();

        NativePtr getRenderPassDesc();
        NativePtr getDevice();
        NativePtr getCommandBuffer();
        NativePtr getRenderEncoder();

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
            MeshHandle handle, float alpha);

    public:
        RenderContext context;

    private:
        MeshManager& meshManager;
        TextureManager& textureManager;
        ShaderManager& shaderManager;
        World& world;

        TaskTime ema;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_SYSTEM_HPP