#ifndef MODERNBOY_RENDER_COMMAND_HPP
#define MODERNBOY_RENDER_COMMAND_HPP

#include <array>
#include <optional>
#include <variant>
#include "fwd.hpp"
#include "raw_resource.hpp"
#include "common/type.hpp"

namespace ModernBoy::Render
{
    struct FrameStartCommand{
        Vec4 clearColor;
    };
    struct SetViewCommand{
        Transform transform;
        Camera camera;
    };
    struct SetShaderCommand{
        ShaderHandle shaderHandle;
    };
    struct DrawMeshCommand{
        Transform transform;
        MeshHandle meshHandle;
    };
    struct FrameEndCommand{
    };

    using RenderCommand = std::variant<
        FrameStartCommand,
        SetViewCommand,
        SetShaderCommand,
        DrawMeshCommand,
        FrameEndCommand
    >;
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_COMMAND_HPP