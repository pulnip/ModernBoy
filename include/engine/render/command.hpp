#ifndef MODERNBOY_RENDER_COMMAND_HPP
#define MODERNBOY_RENDER_COMMAND_HPP

#include <array>
#include <optional>
#include <variant>
#include "engine/fwd.hpp"
#include "core/math/type.hpp"

namespace ModernBoy::Render
{
    using RenderEpoch = uint32_t;

    struct FrameStartCommand{
        RenderEpoch epoch;
        Vec4 clearColor;
    };
    struct SetViewCommand{
        RenderEpoch epoch;
        DEFINE_TRANSFORM;
        CameraType type;
        float fov;
        float nearPlane;
        float farPlane;
        Projection projection;
    };
    struct SetShaderCommand{
        RenderEpoch epoch;
        NativePtr shader;
    };
    struct SetTextureCommand{
        RenderEpoch epoch;
        NativePtr texture;
    };
    struct DrawMeshCommand{
        RenderEpoch epoch;
        DEFINE_TRANSFORM;
        NativePtr mesh;
    };
    struct FrameEndCommand{
        RenderEpoch epoch;
    };

    using RenderCommand = std::variant<
        FrameStartCommand,
        SetViewCommand,
        SetShaderCommand,
        SetTextureCommand,
        DrawMeshCommand,
        FrameEndCommand
    >;
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_COMMAND_HPP