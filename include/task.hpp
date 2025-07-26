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
        DEFINE_TRANSFORM;
        float alpha;
        MeshHandle meshHandle;
        TextureHandle texHandle;
        ShaderHandle shaderHandle;
    };

    bool operator<(const DrawTask& lhs, const DrawTask& rhs);

    struct ViewTask{
        DEFINE_TRANSFORM;

        CameraType type;
        float fov;
        float nearPlane;
        float farPlane;
        Projection projection;
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