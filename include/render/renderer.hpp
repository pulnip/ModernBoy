#ifndef MODERNBOY_RENDERER_HPP
#define MODERNBOY_RENDERER_HPP

#include <thread>
#include <mutex>
#include <SDL3/SDL_video.h>
#include "fwd.hpp"
#include "lock_free_queue.hpp"
#include "backends/metal/context.hpp"

namespace ModernBoy{ namespace Render{
    template<typename... Fs>
    struct Overload: Fs...{ using Fs::operator()...; };
    template<typename... Fs>
    Overload(Fs...) -> Overload<Fs...>;

    class Renderer{
    public:
        RenderContext context;

    private:
        AppState& app;

        LockFreeQueue<RenderCommand<Mesh, Shader>> queue;

        std::stop_source stsrc;
        std::jthread commandThread;
        std::jthread renderThread;

    public:
        Renderer(SDL_Window* window, AppState& app);
        ~Renderer();

        void renderStart();

    private:
        void produceCommand(std::stop_token stoken);
        void consumeCommand(std::stop_token stoken);
    };
}}

#endif // MODERNBOY_RENDERER_HPP