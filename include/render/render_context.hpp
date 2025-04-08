#ifndef __INC_RENDER_CONTEXT_HPP
#define __INC_RENDER_CONTEXT_HPP

#include <concepts>
#include "resource_handle.hpp"

namespace ModernBoy
{
    template<typename Ctx, typename Mesh>
    concept Renderable = requires(Ctx context, ResourceHandle<Mesh> handle) {
        { context.render(handle) } -> std::same_as<void>;
    };
    
    template<typename Ctx>
    concept RenderContext = requires(Ctx context) {
        { context.beginFrame() } -> std::same_as<void>;
        { context.endFrame() } -> std::same_as<void>;
    } && Renderable<Ctx, typename Ctx::MeshType>;
}

#endif // __INC_RENDER_CONTEXT_HPP