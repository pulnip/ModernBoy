#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include "resource_handle.hpp"

namespace ModernBoy
{
    template<typename T>
    struct ResourceComponent{
        EntityID owner;
        ResourceHandle<T> resourceHandle;

        bool enable = true;
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP