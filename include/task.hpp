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
    };
    struct ViewTask{
        Transform transform;
        Camera camera;
    };
    struct InputTask{
        Input::Button button;
        Input::ButtonState condition;
        std::string behaviour;

        // ToDo.
        Transform transform;
    };
    std::vector<InputTask> toTasks(const Input::InputMap& map);
} // namespace ModernBoy::Render

#endif // MODERNBOY_TASK_HPP