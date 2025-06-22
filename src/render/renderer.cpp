#include "render/renderer.hpp"
#include "app_state.hpp"

using namespace ModernBoy;

Renderer::Renderer(SDL_Window* window, AppState& app)
:context(window, app),app(app){}

Renderer::~Renderer(){ stsrc.request_stop(); }

void Renderer::renderStart(){
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

void Renderer::produceCommand(std::stop_token stoken){
    while(!stoken.stop_requested()){
        auto viewTasks = app.get<ViewTask>();
        auto renderTasks = app.get<RenderTask>();

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

void Renderer::consumeCommand(std::stop_token stoken){
    while(!stoken.stop_requested()){
        RenderCommand<Mesh, Shader> cmd;
        waitUntilPopped(queue, cmd, stoken);

        std::visit(context, cmd);
    }
}
