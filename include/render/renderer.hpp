#ifndef MODERNBOY_RENDERER_HPP
#define MODERNBOY_RENDERER_HPP

#include <thread>
#include <mutex>
#include "lock_free_queue.hpp"
#include "render_context.hpp"
#include "resource_manager.hpp"
#include "task_system.hpp"
#include "render/component.hpp"
#include "fwd.hpp"

namespace ModernBoy{ namespace Render{
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

        using MeshComponent = Component<Mesh>;
        using RenderSystem = TaskSystem<MeshComponent, Task<Mesh>>;

        LockFreeQueue<RenderCommand<Mesh, Shader>> queue;
        const RenderSystem& renderTaskManager;
        const ViewSystem& viewTaskManager;

        std::stop_source stsrc;
        std::jthread commandThread;
        std::jthread renderThread;

    public:
        Renderer(Window* window, TransformManager& transformManager,
            ResourceManager<Mesh>& meshManager,
            ResourceManager<Shader>& shaderManager,
            RenderSystem& renderTaskManager,
            CameraManager& cameraManager,
            ViewSystem& viewTaskManager, UI* gui)
        :context(window, transformManager, meshManager, shaderManager,
            cameraManager, gui), renderTaskManager(renderTaskManager),
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
                    waitUntilPushed(queue, FrameStartCommand<Shader>{
                        // TODO: for multiple scene viewport
                        .shaderHandle = { .index=0, .generation=1 },
                        .cameraTransformHandle = task.transformHandle,
                        .cameraHandle = task.cameraHandle,
                    }, stoken);
                }

                for(auto& task: renderTasks){
                    waitUntilPushed(queue, DrawCommand<Mesh>{
                        .transformHandle = task.transformHandle,
                        .meshHandle = task.meshHandle
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
}}

#endif // MODERNBOY_RENDERER_HPP