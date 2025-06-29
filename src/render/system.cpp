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
commandThread(
    std::jthread(
        [this](std::stop_token stoken){
            produceCommand(stoken);
        }, stsrc.get_token()
    )
),
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

static Tasks fetchTask(const ArchetypeMap& map);
static void sortTask(RenderTasks& tasks);
static void setFrameStart(RenderQueue& queue,
    std::stop_token stoken);
static void setFrameEnd(RenderQueue& queue,
    std::stop_token stoken);

thread_local RenderEpoch localEpoch = 0;

void System::produceCommand(std::stop_token stoken){
    while(!stoken.stop_requested()){
        auto [viewTasks, renderTasks] = fetchTask(app.archetypeMap);
        sortTask(renderTasks);

        setFrameStart(commandQueue, stoken);

        for(const auto& view: viewTasks){
            setView(view, stoken);

            auto shaderHandle = invalidResourceHandle();
            auto textureHandle = invalidResourceHandle();
            auto meshHandle = invalidResourceHandle();
            for(const auto& renderTask: renderTasks){
                if(renderTask.shaderHandle != shaderHandle){
                    shaderHandle = renderTask.shaderHandle;
                    setShader(shaderHandle, stoken);
                }
                if(renderTask.texHandle != textureHandle){
                    textureHandle = renderTask.texHandle;
                    setTexture(textureHandle, stoken);
                }
                if(renderTask.meshHandle != meshHandle){
                    meshHandle = renderTask.meshHandle;
                    drawMesh(renderTask.transform, meshHandle, stoken);
                }
            }
        }
        setFrameEnd(commandQueue, stoken);
        lastCompleted.store(localEpoch, std::memory_order_release);

        localEpoch = (localEpoch+1) < 256 ?
            localEpoch+1 : 0;
    }
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

    while(!stoken.stop_requested()){
        RenderCommand cmd;
        waitUntilPopped(commandQueue, cmd, stoken);

        std::visit(*this, cmd);
    }
}
void System::operator()(const Render::FrameStartCommand& cmd
){
    if(cmd.epoch != localEpoch)
        return;
    context.onFrameStart(cmd.clearColor);
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow(); // Show demo window! :)

    app.ui.update();
}
void System::operator()(const Render::SetViewCommand& cmd
){
    if(cmd.epoch != localEpoch)
        return;
    const auto& cameraTransform = cmd.transform;
    const auto& viewPos = cameraTransform.position;
    const auto& viewQuat = cameraTransform.rotation;
    const auto& camera = cmd.camera;

    context.setView(camera.fov, viewPos, viewQuat);
}
void System::operator()(const Render::SetShaderCommand& cmd
){
    if(cmd.epoch != localEpoch)
        return;
    assert(cmd.shader != nullptr);
    context.setShader(cmd.shader);
}
void System::operator()(const Render::SetTextureCommand& cmd
){
    if(cmd.epoch != localEpoch)
        return;
    assert(cmd.texture != nullptr);
    context.setTexture(cmd.texture);
}
void System::operator()(const Render::DrawMeshCommand& cmd
){
    if(cmd.epoch != localEpoch)
        return;
    // auto now = steady_clock::now().time_since_epoch();
    // float seconds = duration<float>(now).count();
    // float ry = fmodf(seconds * (float)(std::numbers::pi/2.0), (float)(std::numbers::pi * 2.0));
    context.drawMesh(cmd.transform, cmd.mesh);
}
void System::operator()(
    [[maybe_unused]] const Render::FrameEndCommand& cmd
){
    if(cmd.epoch != localEpoch){
        localEpoch = lastCompleted.load(std::memory_order_acquire);
        return;
    }
    ImGui::Render();
    context.onFrameEnd(ImGui::GetDrawData());
}

static Tasks fetchTask(const ArchetypeMap& map){
    ViewTasks viewTasks;
    RenderTasks renderTasks;

    for(const auto& [bit, vec]: map){
        if(subset(bit_of<ViewTask>(), bit)){
            viewTasks.reserve(viewTasks.size()+vec.size());
            vec.for_each([&viewTasks, bit](const void* chunk){
                TransformComponent tc;
                CameraComponent cc;
                getChunk(&tc, &cc, nullptr, nullptr, chunk, bit);

                assert(tc.actor == cc.actor);
                if(cc.isActive)
                    viewTasks.emplace_back(ViewTask{
                        tc.value, cc.value
                    });
            });
        }
        else if(subset(bit_of<RenderTask>(), bit)){
            renderTasks.reserve(renderTasks.size()+vec.size());
            vec.for_each([&renderTasks, bit](const void* chunk){
                TransformComponent tc;
                MeshComponent mc;
                getChunk(&tc, nullptr, &mc, nullptr, chunk, bit);

                assert(tc.actor == mc.actor);
                renderTasks.emplace_back(RenderTask{
                    tc.value, mc.handle,
                    mc.textureHandle,
                    mc.shaderHandle
                });
            });
        }
    }
    return {viewTasks, renderTasks};
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

