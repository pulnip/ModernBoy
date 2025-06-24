#ifndef MODERNBOY_RESOURCE_HANDLE_HPP
#define MODERNBOY_RESOURCE_HANDLE_HPP

#include <cstdint>
#include <cstddef>

namespace ModernBoy
{
    struct Handle{
        size_t index = size_t(-1);
        uint32_t generation = 0;

        static constexpr Handle invalid(){ return Handle{}; }
    
        bool isValid() const{ return index != UINT32_MAX; }
        bool operator==(const Handle&) const = default;
    };

    enum class ResourceType{
        INVALID = -1,
        MESH = 0,
        SHADER = 1,
        INPUT = 2,
        NUM_RESOURCE = 3,
    };

    struct ResourceHandle{
        ResourceType type = ResourceType::INVALID;

        size_t index = size_t(-1);
        uint32_t generation = 0;
    
        static constexpr ResourceHandle invalid(){ return ResourceHandle{}; }
    
        bool isValid() const{ return index != size_t(-1); }
        bool operator==(const ResourceHandle&) const = default;
    };
} // namespace ModernBoy

#endif // MODERNBOY_RESOURCE_HANDLE_HPP