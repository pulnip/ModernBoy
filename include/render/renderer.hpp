#ifndef __INC_RENDERER_HPP
#define __INC_RENDERER_HPP

#include <thread>
#include <mutex>
#include "lock_free_queue.hpp"
#include "render_context.hpp"
#include "resource_manager.hpp"

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
        using Window = typename Ctx::Window;
        using Mesh = typename Ctx::Mesh;

        using MeshComponent = ResourceComponent<Mesh>;
        using MeshComponentSystem = ComponentSystem<MeshComponent>;

        LockFreeQueue<RenderCommand<Mesh>> queue;
        const MeshComponentSystem& meshComponentSystem;

        std::stop_source stsrc;
        std::jthread commandThread;
        std::jthread renderThread;

    public:
        Renderer(Window* window, ResourceManager<Mesh>& meshManager,
            const MeshComponentSystem& meshComponentSystem)
        :context(window, meshManager), meshComponentSystem(meshComponentSystem),
        commandThread([this](std::stop_token stoken){
            produceCommand(stoken);
        }, stsrc.get_token()),
        renderThread([this](std::stop_token stoken){
            consumeCommand(stoken);
        }, stsrc.get_token()){}

        ~Renderer(){ stsrc.request_stop(); }

    private:
        void produceCommand(std::stop_token stoken){
            while(!stoken.stop_requested()){
                auto components = meshComponentSystem.getAll();

                waitUntilPushed(queue, StartCommand{}, stoken);

                for(const auto& comp: components){
                    waitUntilPushed(queue, DrawCommand<Mesh>{
                        .handle = comp.get().resourceHandle
                    }, stoken);
                }

                waitUntilPushed(queue, ClearCommand{}, stoken);
            }
        }

        void consumeCommand(std::stop_token stoken){
            while(!stoken.stop_requested()){
                RenderCommand<Mesh> cmd;
                waitUntilPopped(queue, cmd);

                std::visit(context, cmd);
            }
        }
    };
} // namespace ModernBoy

#endif // __INC_RENDERER_HPP