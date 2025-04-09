#ifndef __INC_RENDER_COMMAND_HPP
#define __INC_RENDER_COMMAND_HPP

#include <array>
#include <optional>
#include <variant>
#include "resource_handle.hpp"

namespace ModernBoy
{
    struct StartCommand{};

    struct ClearCommand{
        std::optional<std::array<float, 4>> color = std::nullopt;
        bool clearColor = true;
        bool clearDepth = true;
    };

    template<typename Mesh>
    struct DrawCommand{
        ResourceHandle<Mesh> handle;

        ResourceHandle<Mesh> getResource() const{ return handle; }
    };

    template<typename Mesh>
    using RenderCommand = std::variant<
        StartCommand,
        DrawCommand<Mesh>,
        ClearCommand
    >;
} // namespace ModernBoy

#endif // __INC_RENDER_COMMAND_HPP