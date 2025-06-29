#ifndef MODERNBOY_FWD_HPP
#define MODERNBOY_FWD_HPP

#include <chrono>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ModernBoy
{
    // Common Types
    union Vec3;
    union Vec4;
    struct Transform;
    struct Camera;
    using EntityID = uint32_t;
    using ArchetypeBit = uint64_t;
    // for Input-Action
    using ActionID = uint32_t;
    // for UI subscription
    using ObserverID = uint32_t;
    // for script invoker indexing
    using FunctionID = uint32_t;
    // Handles
    struct Handle;
    struct ResourceHandle;
    using MeshHandle = ResourceHandle;
    using ShaderHandle = ResourceHandle;
    // Components
    struct TransformComponent;
    struct CameraComponent;
    struct MeshComponent;
    struct InputComponent;
    struct SparseChunk;
    // Games
    using DeltaTime = std::chrono::milliseconds;

    class AssetLoader;

    // Raw Resource
    struct RawVertex;
    using Vertices = std::vector<RawVertex>;
    using Indices = std::vector<uint32_t>;
    struct RawMeshPart;
    using RawMesh = std::vector<RawMeshPart>;
    struct RawTexture;
    using RawTextures = std::vector<RawTexture>;
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
        struct Texture;
        struct Shader;
        struct RenderContext;
    }
    using Mesh = Metal::Mesh;
    using Texture = Metal::Texture;
    using Shader = Metal::Shader;
    using RenderContext = Metal::RenderContext;
#elif defined(USE_OPENGL)
#endif
    struct AppState;
    class UI;
    // Handles
    // Managers
    template<typename Resource> class ResourceManager;
    // Loaders
    class AssetLoader;
    // Actor
    constexpr auto INVALID_ENTITY = std::numeric_limits<EntityID>::max();
    // Tasks
    struct RenderTask;
    struct ViewTask;
    struct InputTask;

    namespace Input
    {
        struct State;
        class Device;
        class System;
        class Controller;
    }
    namespace Script
    {
        struct Module;
    }
    namespace Render
    {
        struct FrameStartCommand;
        struct SetViewCommand;
        struct SetShaderCommand;
        struct DrawMeshCommand;
        struct FrameEndCommand;
        class Renderer;
    }
    using NativePtr = void*;

    using MeshManager = ResourceManager<Mesh>;
    using TextureManager = ResourceManager<Texture>;
    using ShaderManager = ResourceManager<Shader>;
    using ShaderHandle = ResourceHandle;
    using TextureHandle = ResourceHandle;
    using ModuleManager = ResourceManager<Script::Module>;
    using ModuleHandle = ResourceHandle;

    template<typename Resource>
    Resource& get(
        AppState& app, ResourceHandle handle);
    template<typename Resource>
    Resource& get(
        AppState& app, const std::string& name);
    template<typename Resource>
    ResourceHandle getHandle(
        AppState& app, const std::string& name);

        template<typename Component>
    std::optional<Component> query(
        AppState& app, EntityID actor);
}

#endif // MODERNBOY_FWD_HPP