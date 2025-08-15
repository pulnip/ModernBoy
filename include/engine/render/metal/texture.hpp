#ifndef MODERNBOY_METAL_TEXTURE_HPP
#define MODERNBOY_METAL_TEXTURE_HPP

#include "engine/fwd.hpp"

namespace ModernBoy::Metal
{
    struct Texture{
        NativePtr texture;

        Texture(const std::string& fileName,
            NativePtr layerPtr);
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