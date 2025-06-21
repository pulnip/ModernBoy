#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include "resource_handle.hpp"

namespace ModernBoy
{
    struct TransformComponent{
        EntityID actor;
        TransformHandle transformHandle;
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_COMPONENT_HPP