#ifndef __INC_RENDER_CONTEXT_HPP
#define __INC_RENDER_CONTEXT_HPP

#include <concepts>
#include "render_command.hpp"
#include "resource_handle.hpp"

namespace ModernBoy
{
    template<typename Ctx>
    concept RenderContext = requires(Ctx context) {
        { context(FrameStartCommand<typename Ctx::Shader>()) } -> std::same_as<void>;
        { context(DrawCommand<typename Ctx::Mesh>()) } -> std::same_as<void>;
        { context(FrameEndCommand()) } -> std::same_as<void>;
    };
}

#endif // __INC_RENDER_CONTEXT_HPP