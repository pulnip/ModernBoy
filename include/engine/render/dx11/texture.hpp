#ifndef MODERNBOY_DX11_TEXTURE_HPP
#define MODERNBOY_DX11_TEXTURE_HPP

#include "engine/fwd.hpp"

namespace ModernBoy::DX11
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

#endif // MODERNBOY_DX11_TEXTURE_HPP