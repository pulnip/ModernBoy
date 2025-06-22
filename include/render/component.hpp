#ifndef MODERNBOY_RENDER_COMPONENT_HPP
#define MODERNBOY_RENDER_COMPONENT_HPP

#include <vector>
#include "fwd.hpp"
#include "resource_handle.hpp"

namespace ModernBoy::Render
{
    template<typename Mesh>
    struct Task{
        TransformHandle transformHandle;
        MeshHandle meshHandle;
    };
    template<typename Mesh>
    using Tasks = std::vector<Task<Mesh>>;

    template<typename Mesh>
    struct Component{
        EntityID actor;
        bool enabled = true;
        TransformHandle transformHandle;
        MeshHandle meshHandle;

        Component(EntityID actor, TransformHandle transformHandle,
            MeshHandle meshHandle)
        :actor(actor), transformHandle(transformHandle),
        meshHandle(meshHandle){}

        Tasks<Mesh> getTasks() const{
            Tasks<Mesh> tasks;

            Task<Mesh> newTask{transformHandle, meshHandle};
            if(enabled)
                tasks.emplace_back(std::move(newTask));

            return tasks;
        }
    };
} // namespace ModernBoy::Render

#endif // MODERNBOY_RENDER_COMPONENT_HPP