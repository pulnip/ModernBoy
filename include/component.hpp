#ifndef MODERNBOY_RESOURCE_COMPONENT_HPP
#define MODERNBOY_RESOURCE_COMPONENT_HPP

#include <utility>
#include "fwd.hpp"
#include "resource_handle.hpp"

namespace ModernBoy
{
    struct ResourceComponent{
        EntityID actor = UINT32_MAX;
        ResourceHandle handle;
    };
    template<typename T>
    struct ValueComponent{
        EntityID actor = UINT32_MAX;
        T value;
    };
}
template<> struct std::hash<ModernBoy::ResourceComponent>{
    std::size_t operator()(
        const ModernBoy::ResourceComponent& key
    ) const noexcept{ return key.actor; }
};
template<typename T>
struct std::hash<ModernBoy::ValueComponent<T>>{
    std::size_t operator()(
        const ModernBoy::ValueComponent<T>& key
    ) const noexcept{ return key.actor; }
};


#endif // MODERNBOY_RESOURCE_COMPONENT_HPP