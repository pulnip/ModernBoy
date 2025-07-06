#ifndef MODERNBOY_FWD_HPP
#define MODERNBOY_FWD_HPP

#include <chrono>
#include "common/alias.hpp"
#include "common/type.hpp"

namespace ModernBoy
{
    using EntityID = ID;
    using DeltaTime = std::chrono::microseconds;
    using Timepoint = std::chrono::time_point<
        std::chrono::steady_clock>;

    class AppState;

    namespace UI{
        class UserInterface;
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
    using MeshHandle = ResourceHandle;
    using TextureHandle = ResourceHandle;
    using ShaderHandle = ResourceHandle;
    using ModuleHandle = ResourceHandle;
}

#endif // MODERNBOY_FWD_HPP