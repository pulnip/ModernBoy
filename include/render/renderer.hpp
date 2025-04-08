#ifndef __INC_RENDERER_HPP
#define __INC_RENDERER_HPP

#include <thread>
#include <mutex>
#include "render_context.hpp"
#include "render_queue.hpp"
#include "resource_manager.hpp"

#include <print>

namespace ModernBoy
{
    template<typename... Fs>
    struct Overload: Fs...{ using Fs::operator()...; };
    template<typename... Fs>
    Overload(Fs...) -> Overload<Fs...>;

    template<RenderContext Ctx>
    class Renderer{
    public:
        Ctx context;

    private:
        using WindowType = typename Ctx::WindowType;
        using MeshType = typename Ctx::MeshType;

        RenderQueue<MeshType> queue;

        std::thread renderThread;
        bool running = true;

    public:
        Renderer(WindowType* window, ResourceManager<MeshType>& meshManager,
            MeshSystem<MeshType>& meshSystem)
        :context(window, meshManager), queue(meshSystem),
        renderThread(std::thread(&renderLoop, this)){}
        ~Renderer(){
            running = false;
            if(renderThread.joinable()) renderThread.join();
        }

    private:
        void renderLoop(){
            while(running){
                auto& commands = queue.swapBuffer();

                for(auto& cmd: commands){
                    std::visit(Overload{
                        [&]([[maybe_unused]] const StartCommand&){ context.beginFrame(); },
                        [&](DrawCommand<MeshType>& cmd){ context.render(cmd.handle); },
                        [&]([[maybe_unused]] const ClearCommand& cmd){ context.endFrame();}
                    }, cmd);
                }
            }
        }
    };
} // namespace ModernBoy

#endif // __INC_RENDERER_HPP