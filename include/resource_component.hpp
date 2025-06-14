#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include "resource_handle.hpp"

namespace ModernBoy
{
    template<typename T>
    struct ResourceComponent{
        using Handle = ResourceHandle<T>;

        EntityID owner;
        Handle resourceHandle;

        bool enable = true;

        ResourceComponent(EntityID id, Handle handle)
        :owner(id), resourceHandle(handle){}
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP