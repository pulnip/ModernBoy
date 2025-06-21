#ifndef MODERNBOY_RESOURCE_HANDLE_HPP
#define MODERNBOY_RESOURCE_HANDLE_HPP

#include <cstdint>
#include "raw_resource.hpp"
#include "input/state.hpp"

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
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_HANDLE_HPP