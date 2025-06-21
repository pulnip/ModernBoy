#ifndef MODERNBOY_VIEW_SYSTEM_HPP
#define MODERNBOY_VIEW_SYSTEM_HPP

#include <algorithm>
#include <functional>
#include <vector>
#include "resource_handle.hpp"
#include "task_system.hpp"

namespace ModernBoy
{
    struct ViewTask{
        TransformHandle transformHandle;
        CameraHandle cameraHandle;
    };
    using ViewTasks = std::vector<ViewTask>;

    struct CameraComponent{
        EntityID actor;
        bool enabled = true;
        TransformHandle transformHandle;
        CameraHandle cameraHandle;
        // TODO: how to get viewport of this camera?

        ViewTasks getTasks() const;
    };
    using ViewSystem = TaskSystem<CameraComponent, ViewTask>;
} // namespace ModernBoy

#endif // MODERNBOY_VIEW_SYSTEM_HPP