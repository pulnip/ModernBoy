#ifndef MODERNBOY_FWD_HPP
#define MODERNBOY_FWD_HPP

#include <cstdint>

namespace ModernBoy
{
    struct AppState;
    class UI;
    template<typename Resource> class ResourceManager;
    template<typename T> struct ResourceHandle;
    template<typename Comp, typename Task> class TaskSystem;
    struct ViewTask;
    struct CameraComponent;
    using ViewSystem = TaskSystem<CameraComponent, ViewTask>;

    namespace Input
    {
        class Controller;
    }
    namespace Render
    {
        template<typename Mesh> struct Task;
        template<typename Mesh> struct Component;
        class Renderer;
    }
    using Renderer = Render::Renderer;
    template<typename Shader>
    struct FrameStartCommand;
    template<typename Mesh>
    struct DrawCommand;
    struct FrameEndCOmmand;

    namespace Metal
    {
        struct Mesh;
        struct Shader;
        struct RenderContext;
    }
    using NativePtr = void*;

    using Mesh = Metal::Mesh;
    using Shader = Metal::Shader;
    using RenderContext = Metal::RenderContext;

    using MeshManager = ResourceManager<Mesh>;
    using MeshHandle = ResourceHandle<Mesh>;
    using MeshComponent = Render::Component<Mesh>;
    using ShaderManager = ResourceManager<Shader>;
    using ShaderHandle = ResourceHandle<Shader>;
    using RenderTask = Render::Task<Mesh>;
    using RenderSystem = TaskSystem<
        MeshComponent, RenderTask>;

    using ObserverID = uint32_t;
}

#endif // MODERNBOY_FWD_HPP