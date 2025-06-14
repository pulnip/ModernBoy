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
        using RenderTaskManager = TaskManager<RenderTask<Mesh>>;

        LockFreeQueue<RenderCommand<Mesh, Shader>> queue;
        const RenderTaskManager& renderTaskManager;
        const ViewTaskManager& viewTaskManager;

        std::stop_source stsrc;
        std::jthread commandThread;
        std::jthread renderThread;

    public:
        Renderer(Window* window, TransformManager& transformManager,
            ResourceManager<Mesh>& meshManager,
            ResourceManager<Shader>& shaderManager,
            RenderTaskManager& renderTaskManager,
            CameraManager& cameraManager,
            ViewTaskManager& viewTaskManager)
        :context(window, transformManager, meshManager, shaderManager,
            cameraManager), renderTaskManager(renderTaskManager),
            viewTaskManager(viewTaskManager){}

        ~Renderer(){ stsrc.request_stop(); }

        void renderStart(){
            commandThread = std::jthread(
                [this](std::stop_token stoken){
                    produceCommand(stoken);
                }, stsrc.get_token()
            );
            renderThread = std::jthread(
                [this](std::stop_token stoken){
                    consumeCommand(stoken);
                }, stsrc.get_token()
            );
        }

    private:
        void produceCommand(std::stop_token stoken){
            while(!stoken.stop_requested()){
                auto viewTasks = viewTaskManager.getAll();
                auto renderTasks = renderTaskManager.getAll();

                for(auto& task: viewTasks){
                    if(!task.get().enabled) continue;
                    waitUntilPushed(queue, FrameStartCommand<Shader>{
                        // TODO: for multiple scene viewport
                        .shaderHandle = { .index=0, .generation=1 },
                        .cameraTransformHandle = task.get().getTransformHandle(),
                        .cameraHandle = task.get().getCameraHandle()
                    }, stoken);
                }

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