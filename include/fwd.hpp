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
    struct Handle;

    using DeltaTime = std::chrono::seconds;

    // Raw Resource
    struct RawVertex;
    using Vertices = std::vector<RawVertex>;
    using Indices = std::vector<uint32_t>;
    struct RawMesh;
    using RawMeshes = std::vector<RawMesh>;
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
    template<typename Resource>
    std::vector<Resource> import(AppState& app,
        const std::string& fileName);
    // Loaders
    class AssetLoader;
    // Components
    using ArchetypeBit = uint64_t;
    template<typename T> struct ValueComponent;
    struct ResourceComponent;
    using TransformComponent = ValueComponent<Transform>;
    using CameraComponent = ValueComponent<Camera>;
    struct SparseChunk;
    // Actor
    using EntityID = uint32_t;
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
    }
    namespace Script
    {
        class Controller;
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
    using ShaderManager = ResourceManager<Shader>;
    using ShaderHandle = ResourceHandle;

    using ObserverID = uint32_t;

    template<typename Resource>
    Resource& get(AppState& app,
        ResourceHandle handle);
    template<typename Component>
    std::optional<Component> find(AppState& app,
        EntityID actor);
}

#endif // MODERNBOY_FWD_HPP