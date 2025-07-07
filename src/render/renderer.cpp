#include <algorithm>
#include "log.hpp"
#include "render/renderer.hpp"
#include "app_state.hpp"
#include "task.hpp"
#include "render/command.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Render;

Renderer::Renderer(AppState& app, SDL_Window* window)
:app(app), context(window){}
Renderer::~Renderer(){}

using ViewTasks = std::vector<ViewTask>;
using DrawTasks = std::vector<DrawTask>;
using Tasks = std::pair<ViewTasks, DrawTasks>;
using RenderQueue = LockFreeQueue<RenderCommand>;
using RenderCommands = std::vector<RenderCommand>;

static void sortTask(DrawTasks& tasks);

size_t Renderer::yield_count(){
    size_t numViewTask=0, numDrawTask=0;

    for(const auto& _: app.getBuffer<ViewTask>()){
        ++numViewTask;
    }
    for(const auto& _: app.getBuffer<DrawTask>()){
        ++numDrawTask;
    }

    return numViewTask * numDrawTask + 2;
}

Generator<void> Renderer::updateTask(DeltaTime){
    auto drawTasks = app.getBuffer<DrawTask>();
    sortTask(drawTasks);
    co_yield 0;

    for(const auto& view: app.getBuffer<ViewTask>()){
        setView(view);
        co_yield 0;

        auto shaderHandle = invalidResourceHandle();
        auto textureHandle = invalidResourceHandle();
        for(const auto& draw: drawTasks){
            if(draw.shaderHandle != shaderHandle){
                shaderHandle = draw.shaderHandle;
                setShader(shaderHandle);
            }
            if(draw.texHandle != textureHandle){
                textureHandle = draw.texHandle;
                setTexture(textureHandle);
            }
            drawMesh(draw.transform, draw.meshHandle);
            co_yield 0;
        }
    }
    co_return;
}

void Renderer::onFrameStart(){
    RenderTrace("Frame Start");
    context.onFrameStart(
        {.r=0.0f, .g=0.0f, .b=0.0f, .a=0.5f});
}

void Renderer::setView(const ViewTask& task){
    const auto& cameraTransform = task.transform;
    const auto& viewPos = cameraTransform.position;
    const auto& viewQuat = cameraTransform.rotation;
    const auto& camera = task.camera;
    RenderTrace("Set View, pos: {}, {}, {}",
        viewPos.x, viewPos.y, viewPos.z);

    context.setView(camera.fov, viewPos, viewQuat);
}
void Renderer::setShader(ShaderHandle handle){
    RenderTrace("Set Shader, index: {}", handle.index);
    const auto& shader = app.query<Shader>(handle);
    context.setShader(shader.shaderPtr);
}
void Renderer::setTexture(TextureHandle handle){
    RenderTrace("Set Texture, Index: {}", handle.index);
    const auto& texture = app.query<Texture>(handle);
    RenderTrace("  Texture Ptr: {}", texture.texture);
    context.setTexture(texture.texture);
}
void Renderer::drawMesh(
    const Transform& transform, MeshHandle handle
){
    RenderTrace("draw type: {}, index: {}", static_cast<int>(handle.type), handle.index);
    const auto& mesh = app.query<Mesh>(handle);

    for(const auto partPtr: mesh.meshPtr){
        context.drawMesh(transform, partPtr);
    }
}
void Renderer::onFrameEnd(){
    RenderTrace("Frame End");

    context.onFrameEnd();
}

NativePtr Renderer::getRenderPassDesc(){
    return context.getRenderPassDesc();
}
NativePtr Renderer::getDevice(){
    return context.getDevice();
}
NativePtr Renderer::getCommandBuffer(){
    return context.getCommandBuffer();
}
NativePtr Renderer::getRenderEncoder(){
    return context.getRenderEncoder();
}

static void sortTask(DrawTasks& tasks){
    // sort by shader-texture-mesh order
    std::ranges::sort(tasks,
        [](const auto& lhs, const auto& rhs){
            return lhs.shaderHandle < rhs.shaderHandle ||
                lhs.texHandle < rhs.texHandle ||
                lhs.meshHandle < rhs.meshHandle;
        }
    );
}
