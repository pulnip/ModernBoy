#ifndef __INC_RENDER_COMMAND_HPP
#define __INC_RENDER_COMMAND_HPP

#include <array>
#include <optional>
#include <variant>
#include "resource_handle.hpp"

namespace ModernBoy
{
    template<typename Shader>
    struct FrameStartCommand{
        std::optional<std::array<float, 4>> color = std::nullopt;
        bool clearColor = true;
        bool clearDepth = true;
        ResourceHandle<Shader> shaderHandle;

        ResourceHandle<Shader> getHandle() const{ return shaderHandle; }
    };
    template<typename Mesh>
    struct DrawCommand{
        ResourceHandle<Mesh> meshHandle;

        ResourceHandle<Mesh> getHandle() const{ return meshHandle; }
    };
    struct FrameEndCommand{
    };

    template<typename Mesh, typename Shader>
    using RenderCommand = std::variant<
        FrameStartCommand<Shader>,
        DrawCommand<Mesh>,
        FrameEndCommand
    >;
} // namespace ModernBoy

#endif // __INC_RENDER_COMMAND_HPP