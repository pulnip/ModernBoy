#include <algorithm>
#include "log.hpp"
#include "resource_manager.hpp"
#include "render/renderer.hpp"
#include "app_state.hpp"
#include "task.hpp"
#include "render/command.hpp"

using namespace std::chrono_literals;
using namespace ModernBoy;
using namespace ModernBoy::Render;

Renderer::Renderer(SDL_Window* window,
    MeshManager& meshManager, TextureManager& textureManager,
    ShaderManager& shaderManager, World& world)
:context(window), meshManager(meshManager),
textureManager(textureManager), shaderManager(shaderManager),
world(world), ema(0ms){}

using ViewTasks = std::vector<ViewTask>;
using DrawTasks = std::vector<DrawTask>;
using Tasks = std::pair<ViewTasks, DrawTasks>;
using RenderQueue = LockFreeQueue<RenderCommand>;
using RenderCommands = std::vector<RenderCommand>;

static void sortTask(DrawTasks& tasks);

Generator<void> Renderer::update(DeltaTime){
    auto started = std::chrono::steady_clock::now();

    auto drawTasks = world.getBuffer<DrawTask>();
    sortTask(drawTasks);
    co_yield 0;

    for(const auto& view: world.getBuffer<ViewTask>()){
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
            drawMesh(draw.position, draw.rotation,
                draw.scale, draw.meshHandle);
            co_yield 0;
        }
    }

    auto elapsed = std::chrono::steady_clock::now() - started;
    updateEMA(std::chrono::duration_cast<TaskTime>(elapsed));
    co_return;
}

void Renderer::onFrameStart(){
    RenderTrace("Frame Start");
    context.onFrameStart(
        {.r=0.0f, .g=0.0f, .b=0.0f, .a=0.5f});
}

void Renderer::updateEMA(TaskTime elapsed){
    auto alpha = 1.0 / policy.effective_window_size;
    long long blended = (1-alpha)*ema.count() + alpha*elapsed.count();
    ema = TaskTime(blended);
}

void Renderer::setView(const ViewTask& task){
    const auto& viewPos = task.position;
    const auto& viewQuat = task.rotation;
    const auto& camera = task.camera;
    RenderTrace("Set View, pos: {}, {}, {}",
        viewPos.x, viewPos.y, viewPos.z);

    context.setView(camera.fov, viewPos, viewQuat);
}
void Renderer::setShader(ShaderHandle handle){
    RenderTrace("Set Shader, index: {}", handle.index);
    const auto& shader = shaderManager.get(handle);
    context.setShader(shader.shaderPtr);
}
void Renderer::setTexture(TextureHandle handle){
    RenderTrace("Set Texture, Index: {}", handle.index);
    const auto& texture = textureManager.get(handle);
    RenderTrace("  Texture Ptr: {}", texture.texture);
    context.setTexture(texture.texture);
}
void Renderer::drawMesh(
    const Vec3& position, const Vec4& rotation,
    const Vec3& scale, MeshHandle handle
){
    RenderTrace("draw type: {}, index: {}", static_cast<int>(handle.type), handle.index);
    const auto& mesh = meshManager.get(handle);

    for(const auto partPtr: mesh.meshPtr){
        context.drawMesh(position, rotation, scale, partPtr);
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
