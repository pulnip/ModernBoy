#ifndef MODERNBOY_RENDER_SYSTEM_HPP
#define MODERNBOY_RENDER_SYSTEM_HPP

#include <atomic>
#include <chrono>
#include <map>
#include <thread>
#include <unordered_set>
#include <SDL3/SDL_video.h>
#include "fwd.hpp"
#include "common/alias.hpp"
#include "util/lock_free_queue.hpp"
#include "render/command.hpp"
#if defined(USE_DIRECTX)
#include "backends/dx11/context.hpp"
#elif defined(USE_METAL)
#include "backends/metal/context.hpp"
#elif defined(USE_OPENGL)
#include "backends/opengl/context.hpp"
#endif
#include "util/generator.hpp"

namespace ModernBoy::Render
{

    class System{
    public:
        System(AppState& app);
        ~System();

        size_t yield_count() const noexcept;
        Generator<void> updateTask(DeltaTime dt);
        Generator<void> update(DeltaTime dt);

        // consume command, public for std::visit
        void operator()(const Render::FrameStartCommand&);
        void operator()(const Render::SetViewCommand&);
        void operator()(const Render::SetShaderCommand&);
        void operator()(const Render::SetTextureCommand&);
        void operator()(const Render::DrawMeshCommand&);
        void operator()(const Render::FrameEndCommand&);

    private:
        // produce command
        void setView(const ViewTask& task, std::stop_token stoken);
        void setShader(ShaderHandle handle,
            std::stop_token stoken);
        void setTexture(TextureHandle handle,
            std::stop_token stoken);
        void drawMesh(const Transform& transform,
            MeshHandle handle, std::stop_token stoken);

        void consumeCommand(std::stop_token stoken);

    private:
        AppState& app;

    public:
        RenderContext context;

    private:
        std::vector<ViewTask> viewTasks;
        std::vector<RenderTask> renderTasks;

        LockFreeQueue<RenderCommand> commandQueue;
        std::stop_source stsrc;
        std::atomic<RenderEpoch> lastCompleted = 0;

        // consumer thread
        std::jthread renderThread;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_SYSTEM_HPP