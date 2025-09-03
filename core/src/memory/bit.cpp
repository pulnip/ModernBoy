#include "memory/bit.hpp"

void* ModernBoy::Util::add(void* ptr, size_t nbyte){
    return static_cast<uint8_t*>(ptr)+nbyte;
}
const void* ModernBoy::Util::add(const void* ptr, size_t nbyte){
    return static_cast<const uint8_t*>(ptr)+nbyte;
}

void* ModernBoy::Util::chunkcpy(void* dst, const void* src, size_t size){
    memcpy(dst, src, size);
    return add(dst, size);
}
