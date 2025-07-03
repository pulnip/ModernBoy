#ifndef MODERNBOY_TASK_HPP
#define MODERNBOY_TASK_HPP

#include <vector>
#include "common/type.hpp"
#include "fwd.hpp"
#include "raw_resource.hpp"
#include "input/state.hpp"

namespace ModernBoy
{
    struct RenderTask{
        Transform transform;
        MeshHandle meshHandle;
        TextureHandle texHandle;
        ShaderHandle shaderHandle;
    };

    bool operator<(const RenderTask& lhs, const RenderTask& rhs);

    struct ViewTask{
        Transform transform;
        Camera camera;
    };
    struct InputTask{
        EntityID actor;
        FunctionID function;
        ModuleHandle handle;
        Input::Trigger trigger;
    };
    struct ScriptTask{
        EntityID actor;
        ModuleHandle handle;
        // FunctionID function;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_TASK_HPP