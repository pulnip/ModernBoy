#ifndef MODERNBOY_RENDER_COMMAND_HPP
#define MODERNBOY_RENDER_COMMAND_HPP

#include <array>
#include <optional>
#include <variant>
#include "fwd.hpp"
#include "resource_handle.hpp"
#include "raw_resource.hpp"

namespace ModernBoy::Render
{
    template<typename Shader>
    struct FrameStartCommand{
        std::optional<std::array<float, 4>> color = std::nullopt;
        bool clearColor = true;
        bool clearDepth = true;
        ShaderHandle shaderHandle;

        Transform transform;
        Camera camera;
    };
    template<typename Mesh>
    struct DrawCommand{
        Transform transform;
        MeshHandle meshHandle;
    };
    struct FrameEndCommand{
    };

    template<typename Mesh,
        typename Shader>
    using RenderCommand = std::variant<
        FrameStartCommand<Shader>,
        DrawCommand<Mesh>,
        FrameEndCommand
    >;
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_COMMAND_HPP