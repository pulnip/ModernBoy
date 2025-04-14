#ifndef MODERNBOY_RENDER_COMMAND_HPP
#define MODERNBOY_RENDER_COMMAND_HPP

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

#endif // MODERNBOY_RENDER_COMMAND_HPP