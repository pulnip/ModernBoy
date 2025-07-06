#ifndef MODERNBOY_TASK_HPP
#define MODERNBOY_TASK_HPP

#include <vector>
#include "common/type.hpp"
#include "fwd.hpp"
#include "raw_resource.hpp"
#include "input/state.hpp"

namespace ModernBoy
{
    using FunctionID = ID;

    struct DrawTask{
        Transform transform;
        MeshHandle meshHandle;
        TextureHandle texHandle;
        ShaderHandle shaderHandle;
    };

    bool operator<(const DrawTask& lhs, const DrawTask& rhs);

    struct ViewTask{
        Transform transform;
        Camera camera;
    };
    struct InputTask{
        EntityID actor;
    };
    struct ActionTask{
        EntityID actor;
        ModuleHandle handle;
        FunctionID function;
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_TASK_HPP