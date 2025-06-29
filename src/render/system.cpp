#include <algorithm>
#include "render/system.hpp"
#include "app_state.hpp"
#include "task.hpp"
#include "render/command.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Render;

System::System(AppState& app, SDL_Window* window)
:app(app), context(window, app.ui), commandQueue(),
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
    }
}

void System::consumeCommand(std::stop_token stoken){
    while(!stoken.stop_requested()){
        RenderCommand cmd;
        waitUntilPopped(commandQueue, cmd, stoken);

        std::visit(context, cmd);
    }
}

void System::setView(const ViewTask& task, std::stop_token stoken){
    waitUntilPushed(commandQueue, SetViewCommand{
        // TODO: for multiple scene viewport
        .transform = task.transform,
        .camera = task.camera
    }, stoken);
}
void System::setShader(ShaderHandle handle, std::stop_token stoken){
    auto shader = app.get<Shader>(handle).shaderPtr;
    waitUntilPushed(commandQueue, SetShaderCommand{
        .shader = shader
    }, stoken);
}
void System::setTexture(TextureHandle handle, std::stop_token stoken){
    auto texture = app.get<Texture>(handle).texture;
    waitUntilPushed(commandQueue, SetTextureCommand{
        .texture = texture
    }, stoken);
}
void System::drawMesh(const Transform& transform,
    MeshHandle handle, std::stop_token stoken
){
    auto meshes = app.get<Mesh>(handle).meshPtr;
    for(const auto& mesh: meshes){
        waitUntilPushed(commandQueue, DrawMeshCommand{
            .transform = transform,
            .mesh = mesh
        }, stoken);
    }
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

static void setFrameStart(RenderQueue& queue,
    std::stop_token stoken
){
    waitUntilPushed(queue, FrameStartCommand{
        .clearColor={.r=0.0f, .g=0.0f, .b=0.0f, .a=0.5f}
    }, stoken);
}
static void setFrameEnd(RenderQueue& queue,
    std::stop_token stoken
){
    waitUntilPushed(queue, FrameEndCommand{}, stoken);
}

