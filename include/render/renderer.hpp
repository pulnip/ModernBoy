#ifndef MODERNBOY_RENDERER_HPP
#define MODERNBOY_RENDERER_HPP

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
        using Shader = typename Ctx::Shader;

        using MeshComponent = ResourceComponent<Mesh>;
        using TaskManager_ = TaskManager<RenderTask<Mesh>>;

        LockFreeQueue<RenderCommand<Mesh, Shader>> queue;
        const TaskManager_& taskManager;

        std::stop_source stsrc;
        std::jthread commandThread;
        std::jthread renderThread;

    public:
        Renderer(Window* window, TransformManager& transformManager,
            ResourceManager<Mesh>& meshManager,
            ResourceManager<Shader>& shaderManager,
            const TaskManager_& taskManager)
        :context(window, transformManager, meshManager, shaderManager),
        taskManager(taskManager){}

        ~Renderer(){ stsrc.request_stop(); }

        void renderStart(){
            commandThread = std::jthread(
                [this](std::stop_token stoken){
                    produceCommand(stoken);
                }, stsrc.get_token()
            );
            renderThread= std::jthread(
                [this](std::stop_token stoken){
                    consumeCommand(stoken);
                }, stsrc.get_token()
            );
        }

    private:
        void produceCommand(std::stop_token stoken){
            while(!stoken.stop_requested()){
                auto renderTasks = taskManager.getAll();

                waitUntilPushed(queue, FrameStartCommand<Shader>{
                    // TODO
                    .shaderHandle = { .index=0, .generation=1 }
                }, stoken);

                for(auto& task: renderTasks){
                    waitUntilPushed(queue, DrawCommand<Mesh>{
                        .transformHandle = task.get().getTransformHandle(),
                        .meshHandle = task.get().getMeshHandle()
                    }, stoken);
                }

                waitUntilPushed(queue, FrameEndCommand{}, stoken);
            }
        }

        void consumeCommand(std::stop_token stoken){
            while(!stoken.stop_requested()){
                RenderCommand<Mesh, Shader> cmd;
                waitUntilPopped(queue, cmd, stoken);

                std::visit(context, cmd);
            }
        }
    };
} // namespace ModernBoy

#endif // MODERNBOY_RENDERER_HPP