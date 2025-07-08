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
        ~Renderer();

        size_t yield_count();
        void onFrameStart();
        Generator<void> updateTask(DeltaTime);
        void onFrameEnd();

        NativePtr getRenderPassDesc();
        NativePtr getDevice();
        NativePtr getCommandBuffer();
        NativePtr getRenderEncoder();


    private:
        void setView(const ViewTask& task);
        void setShader(ShaderHandle handle);
        void setTexture(TextureHandle handle);
        void drawMesh(const Transform& transform,
            MeshHandle handle);

    public:
        RenderContext context;

    private:
        MeshManager& meshManager;
        TextureManager& textureManager;
        ShaderManager& shaderManager;
        World& world;

    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_SYSTEM_HPP