#ifndef MODERNBOY_METAL_TEXTURE_HPP
#define MODERNBOY_METAL_TEXTURE_HPP

#include <span>
#include "engine/fwd.hpp"

namespace ModernBoy::Metal
{
    struct Texture{
        NativePtr texture;

        Texture(NativePtr rctxPtr,
            const std::string& fileName);
        Texture(NativePtr rctxPtr,
            std::span<const uint8_t> pixel, int width, int height);
        ~Texture();

        Texture()=default;
        Texture(const Texture&)=delete;
        Texture(Texture&&);
        Texture& operator=(const Texture&)=delete;
        Texture& operator=(Texture&&);

    private:
        // Move semantics
        void moveFrom(Texture&&);
    };
}

#endif // MODERNBOY_METAL_TEXTURE_HPP