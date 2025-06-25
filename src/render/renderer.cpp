#include <print>
#include "render/renderer.hpp"
#include "app_state.hpp"

using namespace ModernBoy;
using namespace Render;

Renderer::Renderer(SDL_Window* window, AppState& app)
:context(window, app), renderThread(
    std::jthread(
        [this](std::stop_token stoken){
            consumeCommand(stoken);
        }, stsrc.get_token()
    )
){}
Renderer::~Renderer(){ stsrc.request_stop(); }

void Renderer::consumeCommand(std::stop_token stoken){
    while(!stoken.stop_requested()){
        RenderCommand cmd;
        waitUntilPopped(queue, cmd, stoken);

        std::visit(context, cmd);
    }
}
