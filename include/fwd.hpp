#ifndef MODERNBOY_FWD_HPP
#define MODERNBOY_FWD_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

struct Vector3;
struct Vector4;
struct Transform;

namespace ModernBoy
{
    // Raw Resource
    struct RawVertex;
    using Vertices = std::vector<RawVertex>;
    using Indices = std::vector<uint32_t>;
    struct RawMesh;
    using RawMeshes = std::vector<RawMesh>;
    struct RawTexture;
    using RawTextures = std::vector<RawTexture>;
    // Values
}
struct Transform;
namespace ModernBoy{
    struct Camera;
    // Value Informations
    enum class Projection;
    // Resource
    using TexPath = std::string; // File Name of Texture
    using TexPaths = std::vector<TexPath>;
#if defined(USE_DIRECTX)
#elif defined(USE_METAL)
    namespace Metal
    {
        struct Mesh;
        struct Shader;
        struct RenderContext;
    }
    using Mesh = Metal::Mesh;
    using Shader = Metal::Shader;
    using RenderContext = Metal::RenderContext;
#elif defined(USE_OPENGL)
#endif
    struct AppState;
    class UI;
    // Handles
    struct ResourceHandle;
    using MeshHandle = ResourceHandle;
    using ShaderHandle = ResourceHandle;
    // Managers
    template<typename Resource> class ResourceManager;
    template<typename RawResource>
    std::vector<RawResource> import(AppState& app,
        const std::string& fileName);
    template<typename Resource>
    ResourceHandle manage(AppState& app,
        Resource&& resource);
    // Loaders
    template<typename RawResource, typename Resource>
    class ResourceLoader;
    class AssetLoader;
    // Components
    template<typename T> struct ValueComponent;
    struct ResourceComponent;
    using TransformComponent = ValueComponent<Transform>;
    using CameraComponent = ValueComponent<Camera>;
    // Actor
    using EntityID = uint32_t;
    // Tasks
    struct ViewTask;
    struct InputTask;
    // Systems
    template<typename Task>
    using Tasks = std::vector<Task>;
    template<typename Task>
    using TaskMap = std::unordered_map<EntityID, Tasks<Task>>;
    using InputSystem = TaskMap<InputTask>;

    namespace Input
    {
        struct State;
        class Controller;
    }
    namespace Render
    {
        template<typename Shader>
        struct FrameStartCommand;
        template<typename Mesh>
        struct DrawCommand;
        struct FrameEndCommand;
        class Renderer;
    }
    using Renderer = Render::Renderer;

    using NativePtr = void*;

    using MeshManager = ResourceManager<Mesh>;
    using MeshLoader = ResourceLoader<RawMesh, Mesh>;
    using ShaderManager = ResourceManager<Shader>;
    using ShaderHandle = ResourceHandle;
    using FrameStartCommand_ = Render::FrameStartCommand<Shader>;
    using DrawCommand_ = Render::DrawCommand<Mesh>;
    using FrameEndCommand_ = Render::FrameEndCommand;

    using ObserverID = uint32_t;

    template<typename Resource>
    Resource& get(AppState& app,
        ResourceHandle handle);
    template<typename Component>
    std::optional<Component> find(AppState& app,
        EntityID actor);

}

#endif // MODERNBOY_FWD_HPP