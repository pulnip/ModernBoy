#ifndef MODERNBOY_FWD_HPP
#define MODERNBOY_FWD_HPP

#include <chrono>
#include "core/alias.hpp"
#include "core/math/type.hpp"

namespace ModernBoy
{
    using EntityID = ID;
    using DeltaTime = std::chrono::microseconds;
    using Timepoint = std::chrono::time_point<
        std::chrono::steady_clock, DeltaTime>;

    using TaskTime = std::chrono::microseconds;

    class Engine;
    class AssetLoader;

    namespace Render{
        class Renderer;
    }
    namespace UI{
        class UserInterface;
    }

    namespace Input{
        class Device;
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
    namespace Script{
        struct Module;
        struct Object;
        class Invoker;
    }
    using Module = Script::Module;
    using Object = Script::Object;
    using ScriptInvoker = Script::Invoker;

    namespace Game{
        class Context;
    }
    using World = Game::Context;

    using MeshHandle = ResourceHandle;
    using TextureHandle = ResourceHandle;
    using ShaderHandle = ResourceHandle;
    using ModuleHandle = ResourceHandle;
    // using ObjectHandle = ResourceHandle;
    using ObjectHandle = HandleV2;

    template<typename Resource>
    class ResourceManager;
    template<typename T>
    class ObjectPoolV2;

    using MeshManager = ResourceManager<Mesh>;
    using TextureManager = ResourceManager<Texture>;
    using ShaderManager = ResourceManager<Shader>;
    using ModuleManager = ResourceManager<Module>;
    // using ObjectManager = ResourceManager<Object>;
    using ObjectManager = ObjectPoolV2<Object>;
}

#endif // MODERNBOY_FWD_HPP