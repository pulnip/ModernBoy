#ifndef MODERNBOY_RESOURCE_HANDLE_HPP
#define MODERNBOY_RESOURCE_HANDLE_HPP

#include <cstdint>

namespace ModernBoy
{
    enum class ResourceType{
        INVALID = -1,
        MESH = 0,
        SHADER = 1,
        INPUT = 2,
        NUM_RESOURCE = 3,
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