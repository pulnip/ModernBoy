#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include "resource_handle.hpp"

namespace ModernBoy
{
    template<typename T>
    struct ResourceComponent{
        using Handle = ResourceHandle<T>;

        EntityID actor;
        Handle resourceHandle;

        bool enable = true;

        ResourceComponent(EntityID id, Handle handle)
        :actor(id), resourceHandle(handle){}
    };
    using TransformComponent = ResourceComponent<Transform>;
    using CameraComponent = ResourceComponent<Camera>;
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP