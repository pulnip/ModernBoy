#ifndef MODERNBOY_RENDERER_HPP
#define MODERNBOY_RENDERER_HPP

#include <thread>
#include <mutex>
#include <SDL3/SDL_video.h>
#include "util/lock_free_queue.hpp"
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
    template<typename... Fs>
    struct Overload: Fs...{ using Fs::operator()...; };
    template<typename... Fs>
    Overload(Fs...) -> Overload<Fs...>;

    class Renderer{
    public:
        RenderContext context;

        // MUST CONSTRUCT queue FIRST
        LockFreeQueue<RenderCommand<Mesh, Shader>> queue;
    private:
        std::jthread renderThread;

    public:
        Renderer(SDL_Window* window, AppState& app);
        ~Renderer() = default;

        void renderStart(std::stop_token stoken);
    private:
        void consumeCommand(std::stop_token stoken);
    };
}

#endif // MODERNBOY_RENDERER_HPP