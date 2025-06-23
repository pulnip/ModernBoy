#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include <utility>
#include "fwd.hpp"
#include "resource_handle.hpp"

namespace ModernBoy
{
    struct Component{
        EntityID actor = UINT32_MAX;
        ResourceHandle handle;
    };
}
template<> struct std::hash<ModernBoy::Component>{
    std::size_t operator()(
        const ModernBoy::Component& key
    ) const noexcept{ return key.actor; }
};


#endif // MODERNBOY_RESOURCE_COMPONENT_HPP