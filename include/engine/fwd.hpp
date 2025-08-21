#ifndef MODERNBOY_FWD_HPP
#define MODERNBOY_FWD_HPP

#include <chrono>
#include <unordered_map>
#include <vector>
#include "core/alias.hpp"
#include "core/math/type.hpp"

struct SDL_Window;
using SDL_MetalView = void*;

namespace ModernBoy
{
    using EntityID = ID;
    using DeltaTime = std::chrono::microseconds;
    using Timepoint = std::chrono::time_point<
        std::chrono::steady_clock, DeltaTime>;

    using TaskTime = std::chrono::microseconds;

    class Engine;
    class AssetLoader;

    namespace Asset{
        class AssetLoader;

        struct SceneDescriptor;
        using ResolveTable = std::unordered_map<std::string, UUID>;
    }

    namespace Render{
        class Renderer;
    }
    namespace UI{
        class UserInterface;
    }

    namespace Input{
        class Device;
    }

    namespace Interface{
        class InputService;
        struct CameraObject;
        class ViewService;
        class DrawService;
        class EngineCommandBus;
    }

    using NativePtr = void*;
#if defined(USE_DIRECTX)
    namespace DX11{
        struct Mesh;
        struct Texture;
        struct Shader;
        struct RenderContext;
    }
    using Mesh = DX11::Mesh;
    using Texture = DX11::Texture;
    using Shader = DX11::Shader;
    using RenderContext = DX11::RenderContext;
#elif defined(USE_METAL)
    namespace Metal
    {
        struct Mesh;
        struct Submesh;
        struct UnlitMaterial;
        struct Texture;
        struct Shader;
        struct RenderContext;
    }
    using Submesh = Metal::Submesh;
    using Mesh = Metal::Mesh;
    using Material = Metal::UnlitMaterial;
    using Texture = Metal::Texture;
    using Shader = Metal::Shader;
    using RenderContext = Metal::RenderContext;
#elif defined(USE_OPENGL)
#endif
    namespace Script{
        struct Module;
        struct Object;
        class Invoker;
        class TypeHelper;
    }
    using Module = Script::Module;
    using Object = Script::Object;
    using ScriptInvoker = Script::Invoker;

    namespace Game{
        class Context;
    }
    using World = Game::Context;

    using MeshHandle = Handle;
    using SubmeshHandle = Handle;
    using TextureHandle = Handle;
    using MaterialHandle = Handle;
    using MaterialSetHandle = Handle;
    using ShaderHandle = Handle;
    using ModuleHandle = Handle;
    using ObjectHandle = Handle;

    // template<typename Resource>
    // class ResourceManager;
    template<typename Resource, typename Key=UUID>
    class ResourceManager;

    using SubmeshManager = ResourceManager<Submesh>;
    using MeshManager = ResourceManager<Mesh>;
    using TextureManager = ResourceManager<Texture>;
    using MaterialTable = std::unordered_map<UUID, std::vector<TextureHandle>>;
    using MaterialManager = ResourceManager<Material>;
    using MaterialSetTable = std::unordered_map<UUID, std::vector<UUID>>;
    using ShaderManager = ResourceManager<Shader>;
    using ModuleManager = ResourceManager<Module, std::string>;
    // using ObjectManager = ResourceManager<Object>;
    using ObjectManager = ResourceManager<Object>;
}

#endif // MODERNBOY_FWD_HPP