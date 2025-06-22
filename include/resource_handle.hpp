#ifndef MODERNBOY_RESOURCE_HANDLE_HPP
#define MODERNBOY_RESOURCE_HANDLE_HPP

#include <cstdint>

namespace ModernBoy
{
    enum ResourceType{
        INVALID = -1,
        TRANSFORM = 0,
        MESH = 1,
        SHADER = 2,
        CAMERA = 3,
        NUM_RESOURCE = 4,
    };

    struct ResourceHandle{
        ResourceType type = ResourceType::INVALID;

        uint32_t index = UINT32_MAX;
        uint32_t generation = 0;
    
        static constexpr ResourceHandle invalid(){ return ResourceHandle{}; }
    
        bool isValid() const{ return index != UINT32_MAX; }
        bool operator==(const ResourceHandle&) const = default;
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_HANDLE_HPP