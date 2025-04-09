#ifndef __INC_RESOURCE_COMPONENT_HPP
#define __INC_RESOURCE_COMPONENT_HPP

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

#endif // __INC_RESOURCE_COMPONENT_HPP