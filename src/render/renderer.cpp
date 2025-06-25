#include <print>
#include "render/renderer.hpp"
#include "app_state.hpp"

using namespace ModernBoy;

Renderer::Renderer(SDL_Window* window, AppState& app)
:context(window, app){}

void Renderer::renderStart(std::stop_token stoken){
    renderThread = std::jthread(
        [this](std::stop_token stoken){
            consumeCommand(stoken);
        }, stoken
    );
}
void Renderer::consumeCommand(std::stop_token stoken){
    while(!stoken.stop_requested()){
        RenderCommand<Mesh, Shader> cmd;
        waitUntilPopped(queue, cmd, stoken);

        std::visit(context, cmd);
    }
}
