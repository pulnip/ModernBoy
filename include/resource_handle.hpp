#ifndef MODERNBOY_RESOURCE_HANDLE_HPP
#define MODERNBOY_RESOURCE_HANDLE_HPP

#include <cstdint>
#include "raw_resource.hpp"

namespace ModernBoy
{
    using EntityID = uint32_t;

    template<typename T>
    struct ResourceHandle{
        using ResourceType = T;
    
        uint32_t index = UINT32_MAX;
        uint32_t generation = 0;
    
        static constexpr ResourceHandle invalid(){ return ResourceHandle{}; }
    
        bool isValid() const{ return index != UINT32_MAX; }
        bool operator==(const ResourceHandle&) const = default;
    };

    using TransformHandle = ResourceHandle<Transform>;
    using CameraHandle = ResourceHandle<Camera>;

    template<typename Mesh>
    struct RenderTask{
        using MeshHandle = ResourceHandle<Mesh>;

        EntityID actor;
        bool enabled = true;
        TransformHandle transformHandle;
        MeshHandle meshHandle;

        RenderTask(EntityID actor, const TransformHandle& th,
            const MeshHandle& mh)
        :actor(actor), transformHandle(th), meshHandle(mh){}

        TransformHandle getTransformHandle() const{ return transformHandle; }
        MeshHandle getMeshHandle() const{ return meshHandle; }
    };

    struct ViewTask{
        EntityID actor;
        bool enabled = true;
        TransformHandle transformHandle;
        CameraHandle cameraHandle;
        // TODO: how to get viewport of this camera?

        ViewTask(EntityID actor, bool enabled,
            const TransformHandle& th, const CameraHandle& ch)
        :actor(actor), enabled(enabled), transformHandle(th),
        cameraHandle(ch){}

        TransformHandle getTransformHandle() const{ return transformHandle; }
        CameraHandle getCameraHandle() const{ return cameraHandle; }
    };

    struct InputTask{
        EntityID actor;
        TransformHandle transformHandle;
        std::string script;

        TransformHandle getTransformHandle() const{ return transformHandle; }
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_HANDLE_HPP