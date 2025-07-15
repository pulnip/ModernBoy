#ifndef MODERNBOY_UTIL_BIT_HPP
#define MODERNBOY_UTIL_BIT_HPP

#include <cstddef>
#include <cstdint>
#include <utility>
#include <print>

namespace ModernBoy::Util
{
    template<typename T>
    T& as(void* ptr){
        return *static_cast<T*>(ptr);
    }
    template<typename T>
    const T& as(const void* ptr){
        return *static_cast<const T*>(ptr);
    }

    template<typename T>
    void* add(void* ptr){
        return static_cast<uint8_t*>(ptr)+sizeof(T);
    }
    template<typename T>
    const void* add(const void* ptr){
        return static_cast<const uint8_t*>(ptr)+sizeof(T);
    }
    void* add(void* ptr, size_t nbyte);
    const void* add(const void* ptr, size_t nbyte);

    template<typename T>
    void* chunkcpy(void* dst, const T& src){
        // std::println("save to {} chunk: {}bytes", dst, sizeof(T));
        *static_cast<T*>(dst) = src;
        return add<T>(dst);
    }
    template<typename T>
    void* chunkcpy(void* dst, T&& src){
        *static_cast<T*>(dst) = std::move(src);
        return add<T>(dst);
    }
    template<typename T>
    const void* chunkcpy(T& dst, const void* src){
        // std::println("load from {} chunk: {}bytes", src, sizeof(T));
        dst = *static_cast<const T*>(src);
        return add<T>(src);
    }

    void* chunkcpy(void* dst, const void* src, size_t size);
}

#endif // MODERNBOY_UTIL_BIT_HPP