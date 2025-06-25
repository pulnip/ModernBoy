#include "render/system.hpp"
#include "app_state.hpp"
#include "task.hpp"
#include "render/command.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Render;

System::System(AppState& app, SDL_Window* window)
:app(app),renderer(window, app){
    commandThread = std::jthread(
        [this](std::stop_token stoken){
            update(stoken);
        }, stsrc.get_token()
    );
    renderer.renderStart(stsrc.get_token());
}

using ViewTasks = std::vector<ViewTask>;
using RenderTasks = std::vector<RenderTask>;
using Tasks = std::pair<ViewTasks, RenderTasks>;
using RenderQueue = std::decay_t<decltype(Renderer::queue)>;
using RenderCommands = std::vector<RenderCommand<Mesh, Shader>>;

static Tasks fetchTask(const ArchetypeMap& map);
static void sortTask(RenderTasks& tasks);
static void setFrameStart(RenderQueue& queue,
    const ViewTasks& tasks, std::stop_token stoken);
static void drawMeshes(RenderQueue& queue,
    const RenderTasks& tasks, std::stop_token stoken);
static void setFrameEnd(RenderQueue& queue,
    std::stop_token stoken);
static std::pair<size_t, size_t> countTask(const ArchetypeMap& map);

void System::update(std::stop_token stoken){
    auto& commandQueue = renderer.queue;

    while(!stoken.stop_requested()){
        auto [viewTasks, renderTasks] = fetchTask(app.archetypeMap);
        sortTask(renderTasks);

        setFrameStart(renderer.queue, viewTasks, stoken);
        drawMeshes(renderer.queue, renderTasks, stoken);
        setFrameEnd(renderer.queue, stoken);
    }
}

static Tasks fetchTask(const ArchetypeMap& map){
    auto [vt_size, rt_size] = countTask(map);
    ViewTasks viewTasks(vt_size);
    RenderTasks renderTasks(rt_size);

    for(const auto& [bit, vec]: map){
        if(subset(bit_of<ViewTask>(), bit))
            for(const auto& chunk: vec){
                TransformComponent tc;
                CameraComponent cc;
                getChunk(&tc, &cc, nullptr, nullptr, chunk);
                assert(tc.actor == cc.actor);
            }
        else if(subset(bit_of<RenderTask>(), bit))
            for(const auto& chunk: vec){
                TransformComponent tc;
                MeshComponent mc;
                getChunk(&tc, nullptr, &mc, nullptr, chunk);
                assert(tc.actor == mc.actor);
            }
    }
    return {viewTasks, renderTasks};
}

static std::pair<size_t, size_t> countTask(const ArchetypeMap& map){
    size_t numView = 0;
    size_t numRender = 0;

    for(const auto& [bit, vec]: map){
        if(subset(bit_of<ViewTask>(), bit))
            numView += vec.size();
        else if(!subset(bit_of<RenderTask>(), bit))
            numRender += vec.size();
    }
    return {numView, numRender};
}

static void sortTask(RenderTasks& tasks){
    // sort by texture-mesh order
}

static void setFrameStart(RenderQueue& queue,
    const ViewTasks& tasks, std::stop_token stoken
){
    for(auto& task: tasks){
        waitUntilPushed(queue, FrameStartCommand<Shader>{
            // TODO: for multiple scene viewport
            .shaderHandle = {.type=ResourceType::SHADER,
                .index=0, .generation=1
            },
            .transform = task.transform,
            .camera = task.camera,
        }, stoken);
    }
}

static void drawMeshes(RenderQueue& queue,
    const RenderTasks& tasks, std::stop_token stoken
){
    for(auto& task: tasks){
        waitUntilPushed(queue, DrawCommand<Mesh>{
            .transform = task.transform,
            .meshHandle = task.meshHandle
        }, stoken);
    }
}

static void setFrameEnd(RenderQueue& queue,
    std::stop_token stoken
){
    waitUntilPushed(queue, FrameEndCommand{}, stoken);
}

