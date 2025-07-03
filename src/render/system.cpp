#include <algorithm>
#include <SDL3/SDL_log.h>
#include <imgui.h>
#include "render/system.hpp"
#include "app_state.hpp"
#include "task.hpp"
#include "render/command.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Render;

System::System(AppState& app)
:app(app), context(app.window), commandQueue(),
renderThread(
    std::jthread(
        [this](std::stop_token stoken){
            consumeCommand(stoken);
        }, stsrc.get_token()
    )
){}
System::~System(){ stsrc.request_stop(); }

using ViewTasks = std::vector<ViewTask>;
using RenderTasks = std::vector<RenderTask>;
using Tasks = std::pair<ViewTasks, RenderTasks>;
using RenderQueue = LockFreeQueue<RenderCommand>;
using RenderCommands = std::vector<RenderCommand>;

static void sortTask(RenderTasks& tasks);
static void setFrameStart(RenderQueue& queue,
    std::stop_token stoken);
static void setFrameEnd(RenderQueue& queue,
    std::stop_token stoken);

thread_local RenderEpoch localEpoch = 0;

size_t System::yield_count() const noexcept{
    size_t numRenderTask = 0, numViewTask = 0;

    for(const auto& [bit, vec]: app.archetypeMap){
        if(subset(bit_of<ViewTask>(), bit))
            numViewTask += 1;
        if(subset(bit_of<RenderTask>(), bit))
            numRenderTask += 1;
    }

    return numViewTask * numRenderTask;
}

Generator<void> System::updateTask(DeltaTime){
    viewTasks.clear();
    renderTasks.clear();

    for(const auto& [bit, vec]: app.archetypeMap){
        if(subset(bit_of<ViewTask>(), bit)){
            viewTasks.reserve(vec.size());

            vec.for_each([this](
                const TransformComponent& tc,
                const CameraComponent& cc
            ){
                assert(tc.actor == cc.actor);
                if(cc.isActive)
                    viewTasks.emplace_back(ViewTask{
                        tc.value, cc.value});
            });
            co_yield 0;
        }
        if(subset(bit_of<RenderTask>(), bit)){
            renderTasks.reserve(vec.size());

            vec.for_each([this](
                const TransformComponent& tc,
                const MeshComponent& mc
            ){
                assert(tc.actor == mc.actor);
                if(mc.isActive)
                    renderTasks.emplace_back(RenderTask{
                        tc.value, mc.handle,
                        mc.textureHandle,
                        mc.shaderHandle});
            });
                co_yield 0;
        }
    }
    co_return;
}

Generator<void> System::update(DeltaTime){
    sortTask(renderTasks);
    setFrameStart(commandQueue, stsrc.get_token());
    co_yield 0;

    for(const auto& view: viewTasks){
        setView(view, stsrc.get_token());
        co_yield 0;

        auto shaderHandle = invalidResourceHandle();
        auto textureHandle = invalidResourceHandle();
        for(const auto& renderTask: renderTasks){
            if(renderTask.shaderHandle != shaderHandle){
                shaderHandle = renderTask.shaderHandle;
                setShader(shaderHandle, stsrc.get_token());
            }
            if(renderTask.texHandle != textureHandle){
                textureHandle = renderTask.texHandle;
                setTexture(textureHandle, stsrc.get_token());
            }
            drawMesh(renderTask.transform,
                renderTask.meshHandle, stsrc.get_token());
            co_yield 0;
        }
    }
    setFrameEnd(commandQueue, stsrc.get_token());
    lastCompleted.store(localEpoch, std::memory_order_release);

    localEpoch = (localEpoch+1) < 256 ?
        localEpoch+1 : 0;
    co_return;
}

void System::setView(const ViewTask& task, std::stop_token stoken){
    waitUntilPushed(commandQueue, SetViewCommand{
        .epoch = localEpoch,
        // TODO: for multiple scene viewport
        .transform = task.transform,
        .camera = task.camera
    }, stoken);
}
void System::setShader(ShaderHandle handle, std::stop_token stoken){
    auto shader = app.get<Shader>(handle).shaderPtr;
    waitUntilPushed(commandQueue, SetShaderCommand{
        .epoch = localEpoch,
        .shader = shader
    }, stoken);
}
void System::setTexture(TextureHandle handle, std::stop_token stoken){
    auto texture = app.get<Texture>(handle).texture;
    waitUntilPushed(commandQueue, SetTextureCommand{
        .epoch = localEpoch,
        .texture = texture
    }, stoken);
}
void System::drawMesh(const Transform& transform,
    MeshHandle handle, std::stop_token stoken
){
    auto meshes = app.get<Mesh>(handle).meshPtr;
    for(const auto& mesh: meshes){
        waitUntilPushed(commandQueue, DrawMeshCommand{
            .epoch = localEpoch,
            .transform = transform,
            .mesh = mesh
        }, stoken);
    }
}

static void setFrameStart(RenderQueue& queue,
    std::stop_token stoken
){
    waitUntilPushed(queue, FrameStartCommand{
        .epoch = localEpoch,
        .clearColor={.r=0.0f, .g=0.0f, .b=0.0f, .a=0.5f}
    }, stoken);
}
static void setFrameEnd(RenderQueue& queue,
    std::stop_token stoken
){
    waitUntilPushed(queue, FrameEndCommand{
        .epoch = localEpoch
    }, stoken);
}

void System::consumeCommand(std::stop_token stoken){
    context.initialize(app.window);

    int w, h;
    if(!SDL_GetWindowSize(app.window, &w, &h)){
        SDL_Log("SDL_GetWindowSize Failed: %s", SDL_GetError());
        throw 1;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(w, h);
    // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    // ImGui::StyleColorsLight();
    ImGui::StyleColorsDark();

    bool decideToDraw = false;

    while(!stoken.stop_requested()){
        RenderCommand cmd;
        // pop until cmd.epoch == localEpoch
        while(!stoken.stop_requested()){
            waitUntilPopped(commandQueue, cmd, stoken);
            if(std::holds_alternative<FrameStartCommand>(cmd)){
                if(std::get<FrameStartCommand>(cmd).epoch==localEpoch){
                    decideToDraw = true;
                    break;
                }
                else decideToDraw = false;
            }
            else if(decideToDraw) break;
        }

        std::visit(*this, cmd);

        if(std::holds_alternative<FrameEndCommand>(cmd)){
            // wait until next command is completed
            while(!stoken.stop_requested()){
                auto globalEpoch = lastCompleted.load(std::memory_order_acquire);
                if(localEpoch != globalEpoch){
                    localEpoch = globalEpoch;
                    break;
                }
            }
        }
    }
}
void System::operator()(const Render::FrameStartCommand& cmd
){
    context.onFrameStart(cmd.clearColor);
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow(); // Show demo window! :)

    app.ui.update();
}
void System::operator()(const Render::SetViewCommand& cmd
){
    const auto& cameraTransform = cmd.transform;
    const auto& viewPos = cameraTransform.position;
    const auto& viewQuat = cameraTransform.rotation;
    const auto& camera = cmd.camera;

    context.setView(camera.fov, viewPos, viewQuat);
}
void System::operator()(const Render::SetShaderCommand& cmd
){
    assert(cmd.shader != nullptr);
    context.setShader(cmd.shader);
}
void System::operator()(const Render::SetTextureCommand& cmd
){
    assert(cmd.texture != nullptr);
    context.setTexture(cmd.texture);
}
void System::operator()(const Render::DrawMeshCommand& cmd
){
    context.drawMesh(cmd.transform, cmd.mesh);
}
void System::operator()(
    [[maybe_unused]] const Render::FrameEndCommand& cmd
){
    ImGui::Render();
    context.onFrameEnd(ImGui::GetDrawData());
}

static void sortTask(RenderTasks& tasks){
    // sort by shader-texture-mesh order
    std::ranges::sort(tasks,
        [](const auto& lhs, const auto& rhs){
            return lhs.shaderHandle < rhs.shaderHandle ||
                lhs.texHandle < rhs.texHandle ||
                lhs.meshHandle < rhs.meshHandle;
        }
    );
}

